// WIP requirements
// - Each packet fits within 256 byte page
// - Checksum at the end of data content (maybe uint32_t)
// - Writes using regular SPI (not QSPI), no DMA
// - Consider what happens if a write operation is interrupted (e.g., power loss)
// - Consider if flash is full when writing


// For reading contents, create empty instance of packet struct and memcpy received buffer into it

HAL_StatusTypeDef write_page(uint32_t address, Packet *packet_data) {
    // Datsheet page 34:
    // "If an entire 256 byte page is to be programmed, the last address byte (the 8 least significant address bits) should be set to 0"

	// Ensure page address is aligned to 256 bytes (page size)
	if (address % 256 != 0)
		return HAL_ERROR;

    return write_data(
        address,
        (uint8_t *)packet_data,
        sizeof(Packet)
    );
}
