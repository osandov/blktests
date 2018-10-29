// SPDX-License-Identifier: GPL-2.0+
// Copyright (c) 2015 SanDisk Corporation
// Copyright (c) 2016-2018 Western Digital Corporation or its affiliates

#include <cassert>
#include <cstring>     // memset()
#include <fcntl.h>     // O_RDONLY
#include <iomanip>
#include <iostream>
#include <linux/fs.h>  // BLKSSZGET
#include <scsi/sg.h>   // sg_io_hdr_t
#include <sys/ioctl.h>
#include <unistd.h>    // open()
#include <vector>

class file_descriptor {
public:
	file_descriptor(int fd = -1)
		: m_fd(fd)
	{ }
	~file_descriptor()
	{ if (m_fd >= 0) close(m_fd); }
	operator int() const
	{ return m_fd; }

private:
	file_descriptor(const file_descriptor &);
	file_descriptor &operator=(const file_descriptor &);

	int m_fd;
};

class iovec_t {
public:
	iovec_t()
	{ }
	~iovec_t()
	{ }
	size_t size() const
	{ return m_v.size(); }
	const sg_iovec_t& operator[](const int i) const
	{ return m_v[i]; }
	sg_iovec_t& operator[](const int i)
	{ return m_v[i]; }
	void append(void *addr, size_t len) {
		m_v.resize(m_v.size() + 1);
		auto p = m_v.end() - 1;
		p->iov_base = addr;
		p->iov_len = len;
	}
	const void *address() const {
		return &*m_v.begin();
	}
	size_t data_len() const {
		size_t len = 0;
		for (auto p = m_v.begin(); p != m_v.end(); ++p)
			len += p->iov_len;
		return len;
	}
	void trunc(size_t len) {
		size_t s = 0;
		for (auto p = m_v.begin(); p != m_v.end(); ++p) {
			s += p->iov_len;
			if (s >= len) {
				p->iov_len -= s - len;
				assert(p->iov_len > 0 ||
				       (p->iov_len == 0 && len == 0));
				m_v.resize(p - m_v.begin() + 1);
				break;
			}
		}
	}
	std::ostream& write(std::ostream& os) const {
		for (auto p = m_v.begin(); p != m_v.end(); ++p)
			os.write((const char *)p->iov_base, p->iov_len);
		return os;
	}

private:
	iovec_t(const iovec_t &);
	iovec_t &operator=(const iovec_t &);

	std::vector<sg_iovec_t> m_v;
};

static unsigned block_size;

static void dumphex(std::ostream &os, const void *a, size_t len)
{
	for (int i = 0; i < len; i += 16) {
		os << std::hex << std::setfill('0') << std::setw(16)
		   << (uintptr_t)a + i << ':';
		for (int j = i; j < i + 16 && j < len; j++) {
			if (j % 4 == 0)
				os << ' ';
			os << std::hex << std::setfill('0') << std::setw(2)
			   << (unsigned)((uint8_t*)a)[j];
		}
		os << "  ";
		for (int j = i; j < i + 16 && j < len; j++) {
			unsigned char c = ((uint8_t*)a)[j];
			os << (c >= ' ' && c < 128 ? (char)c : '.');
		}
		os << '\n';
	}
}

enum {
	MAX_READ_WRITE_6_LBA = 0x1fffff,
	MAX_READ_WRITE_6_LENGTH = 0xff,
};

static ssize_t sg_read(const file_descriptor &fd, uint32_t lba,
		       const iovec_t &v)
{
	if (lba > MAX_READ_WRITE_6_LBA)
		return -1;

	if (v.data_len() == 0 || (v.data_len() % block_size) != 0)
		return -1;

	if (v.data_len() / block_size > MAX_READ_WRITE_6_LENGTH)
		return -1;

	int sg_version;
	if (ioctl(fd, SG_GET_VERSION_NUM, &sg_version) < 0 ||
	    sg_version < 30000)
		return -1;

	uint8_t read6[6] = {
		0x08, (uint8_t)(lba >> 16), (uint8_t)(lba >> 8),
		(uint8_t)(lba), (uint8_t)(v.data_len() / block_size),
		0
	};
	unsigned char sense_buffer[32];
	sg_io_hdr_t h;

	memset(&h, 0, sizeof(h));
	h.interface_id = 'S';
	h.cmdp = read6;
	h.cmd_len = sizeof(read6);
	h.dxfer_direction = SG_DXFER_FROM_DEV;
	h.iovec_count = v.size();
	h.dxfer_len = v.data_len();
	h.dxferp = const_cast<void*>(v.address());
	h.sbp = sense_buffer;
	h.mx_sb_len = sizeof(sense_buffer);
	h.timeout = 1000;     /* 1000 millisecs == 1 second */
	if (ioctl(fd, SG_IO, &h) < 0) {
		std::cerr << "READ(6) ioctl failed with errno " << errno
			  << '\n';
		return -1;
	}
	uint32_t result = h.status | (h.msg_status << 8) |
		(h.host_status << 16) | (h.driver_status << 24);
	if (result) {
		std::cerr << "READ(6) failed with status 0x" << std::hex
			  << result << "\n";
		if (h.status == 2) {
			std::cerr << "Sense buffer:\n";
			dumphex(std::cerr, sense_buffer, h.sb_len_wr);
		}
		return -1;
	}
	return v.data_len() - h.resid;
}

static ssize_t sg_write(const file_descriptor &fd, uint32_t lba,
			const iovec_t &v)
{
	if (lba > MAX_READ_WRITE_6_LBA)
		return -1;

	if (v.data_len() == 0) {
		std::cerr << "Write buffer is empty.\n";
		return -1;
	}

	if ((v.data_len() % block_size) != 0) {
		std::cerr << "Write buffer size " << v.data_len()
			  << " is not a multiple of the block size "
			  << block_size << ".\n";
		return -1;
	}

	if (v.data_len() / block_size > MAX_READ_WRITE_6_LENGTH) {
		std::cerr << "Write buffer size " << v.data_len()
			  << " > " << MAX_READ_WRITE_6_LENGTH << ".\n";
		return -1;
	}

	int sg_version;
	if (ioctl(fd, SG_GET_VERSION_NUM, &sg_version) < 0) {
		std::cerr << "SG_GET_VERSION_NUM ioctl failed with errno "
			  << errno << '\n';
		return -1;
	}

	if (sg_version < 30000) {
		std::cerr << "Error: sg version 3 is not supported\n";
		return -1;
	}

	uint8_t write6[6] = {
		0x0a, (uint8_t)(lba >> 16), (uint8_t)(lba >> 8),
		(uint8_t)(lba), (uint8_t)(v.data_len() / block_size),
		0
	};
	unsigned char sense_buffer[32];
	sg_io_hdr_t h;

	memset(&h, 0, sizeof(h));
	h.interface_id = 'S';
	h.cmdp = write6;
	h.cmd_len = sizeof(write6);
	h.dxfer_direction = SG_DXFER_TO_DEV;
	h.iovec_count = v.size();
	h.dxfer_len = v.data_len();
	h.dxferp = const_cast<void*>(v.address());
	h.sbp = sense_buffer;
	h.mx_sb_len = sizeof(sense_buffer);
	h.timeout = 1000;     /* 1000 millisecs == 1 second */
	if (ioctl(fd, SG_IO, &h) < 0) {
		std::cerr << "WRITE(6) ioctl failed with errno " << errno
			  << '\n';
		return -1;
	}
	uint32_t result = h.status | (h.msg_status << 8) |
		(h.host_status << 16) | (h.driver_status << 24);
	if (result) {
		std::cerr << "WRITE(6) failed with status 0x" << std::hex
			  << result << "\n";
		if (h.status == 2) {
			std::cerr << "Sense buffer:\n";
			dumphex(std::cerr, sense_buffer, h.sb_len_wr);
		}
		return -1;
	}
	return v.data_len() - h.resid;
}

static void usage()
{
	std::cout << "Usage: [-h] [-l <length_in_bytes>] [-o <lba_in_bytes>] [-s] [-w] <dev>\n";
}

int main(int argc, char **argv)
{
	bool scattered = false, write = false;
	uint32_t offs = 0;
	const char *dev;
	int c;
	std::vector<uint8_t> buf;
	unsigned long len = 512;

	while ((c = getopt(argc, argv, "hl:o:sw")) != EOF) {
		switch (c) {
		case 'l': len = strtoul(optarg, NULL, 0); break;
		case 'o': offs = strtoul(optarg, NULL, 0); break;
		case 's': scattered = true; break;
		case 'w': write = true; break;
		default: usage(); goto out;
		}
	}

	if (argc - optind < 1) {
		std::cerr << "Too few arguments.\n";
		goto out;
	}

	dev = argv[optind];
	buf.resize(len);
	{
		file_descriptor fd(open(dev, O_RDONLY));
		if (fd < 0) {
			std::cerr << "Failed to open " << dev << "\n";
			goto out;
		}
		if (ioctl(fd, BLKSSZGET, &block_size) < 0) {
			std::cerr << "Failed to query block size of " << dev
				  << "\n";
			goto out;
		}
		if (offs % block_size) {
			std::cerr << "LBA is not a multiple of the block size.\n";
			goto out;
		}
		iovec_t iov;
		if (scattered) {
			buf.resize(buf.size() * 2);
			unsigned char *p = &*buf.begin();
			for (int i = 0; i < len / 4; i++)
				iov.append(p + 4 + i * 8,
					   std::min(4ul, len - i * 4));
		} else {
			iov.append(&*buf.begin(), buf.size());
		}
		if (write) {
			for (int i = 0; i < iov.size(); i++) {
				sg_iovec_t& e = iov[i];
				size_t prevgcount = std::cin.gcount();
				if (!std::cin.read((char *)e.iov_base,
						   e.iov_len)) {
					e.iov_len = std::cin.gcount() -
						prevgcount;
					break;
				}
			}
			ssize_t written = sg_write(fd, offs / block_size, iov);
			if (written >= 0)
				std::cout << "Wrote " << written << "/"
					  << iov.data_len()
					  << " bytes of data.\n";
		} else {
			ssize_t read = sg_read(fd, offs / block_size, iov);
			if (read >= 0) {
				std::cerr << "Read " << read
					  << " bytes of data:\n";
				iov.trunc(read);
				iov.write(std::cout);
			}
		}
	}

 out:
	return 0;
}
