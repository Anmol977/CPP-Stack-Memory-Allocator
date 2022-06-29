#include <chrono>
#include <stdio.h>
#include <cstdint>
#include <cassert>
#include <iostream>


#define ALIGNMENT	   8
#define MAX_ALLOC_SIZE 10 * 1024 * 1024

struct AllocationHeader {
	char padding;
};

class MemAllocator {
private:
	void* stack_top     = nullptr;
	std::size_t offset  = 0;

	std::size_t calculate_data_padding(const std::size_t base_address, const std::size_t header_size,const std::size_t alignment) {
		 assert(alignment != 0);
		 std::size_t multiplier    = base_address / alignment + 1
			, address_with_padding = alignment * multiplier
			, calculated_padding   = address_with_padding - base_address;

		std::size_t needed_space = header_size;

		if (calculated_padding < header_size) {
			needed_space -= calculated_padding;

			calculated_padding +=  (needed_space % alignment) > 0 ? 
								   alignment * (1 + (needed_space / alignment)) 
								   : needed_space;
		}
		return calculated_padding;
	}
public:
	MemAllocator() {
		stack_top = malloc(MAX_ALLOC_SIZE);
	}

	void* allocate_memory(uint32_t bytes_to_allocate) {
		const std::size_t current_address = (std::size_t)stack_top + offset;
		const std::size_t padding = calculate_data_padding(current_address, ALIGNMENT, sizeof(AllocationHeader));

		if (offset + padding + bytes_to_allocate > MAX_ALLOC_SIZE) {
			std::cout << "not enough resources to allocated memory \n";
			return nullptr;
		}
		offset += padding;
		const std::size_t next_address   = current_address + padding;
		const std::size_t header_address = next_address - sizeof(AllocationHeader);
		AllocationHeader allocationHeader{ padding };
		AllocationHeader* header_ptr = (AllocationHeader*)header_address;
		header_ptr = &allocationHeader;

		offset += bytes_to_allocate;

		//std::cout << "\t@C " << (void*)current_address << "\t@R " << (void*)next_address << "\tO " << offset << "\tP " << padding << std::endl;

		return (void*)next_address;
	}

	void roll_back_stack(void* ptr) {
		const std::size_t current_address = (std::size_t)ptr;
		const std::size_t header_address = current_address - sizeof(AllocationHeader);
		const AllocationHeader* allocationHeader{ (AllocationHeader*)header_address };

		offset = current_address - allocationHeader->padding - (std::size_t)stack_top;
	}

	~MemAllocator() {
		free(stack_top);
		stack_top = nullptr;
	}

};

int main() {
	MemAllocator* stack_allocator = new MemAllocator();
	auto start = std::chrono::steady_clock::now();
	void* addresses[5000];
	for (long i = 0; i < 5000; i++) {
		addresses[i] = stack_allocator->allocate_memory(sizeof(i));
	}
	auto end = std::chrono::steady_clock::now();
	std::cout << "Elapsed time in microseconds: "<< std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()<< "\n";
	start = std::chrono::steady_clock::now();
	for (long i = 0; i < 5000; i++) {
		addresses[i] = malloc(sizeof(i));
	}
	end = std::chrono::steady_clock::now();
	std::cout << "Elapsed time in microseconds for malloc : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "\n";

}