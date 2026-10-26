// WIP requirements
// - Each packet fits within 256 byte page
// - First packet contains metadata (e.g. number of complete packets, addr of last packet, etc.)
// - Checksum at the end of data content (maybe uint32_t)
// - Writes using regular SPI (not QSPI), no DMA