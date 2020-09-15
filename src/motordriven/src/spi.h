
#ifndef _spi_h_
#define _spi_h_

int transfer(int fd, uint8_t const *tx, uint8_t const *rx, size_t len,uint32_t speed,char verb);
int spi_init();
void hex_dump(const void *src, size_t length, size_t line_size, char *prefix);

#endif
