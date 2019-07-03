#include "vk.h"
#include "tr_local.h"


static void ensure_staging_buffer_allocation(VkDeviceSize size)
{
	if (vk_world.staging_buffer_size >= size)
		return;

	if (vk_world.staging_buffer != VK_NULL_HANDLE)
		vkDestroyBuffer(vk.device, vk_world.staging_buffer, NULL);

	if (vk_world.staging_buffer_memory != VK_NULL_HANDLE)
		vkFreeMemory(vk.device, vk_world.staging_buffer_memory, NULL);

	vk_world.staging_buffer_size = size;

	VkBufferCreateInfo buffer_desc;
	buffer_desc.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_desc.pNext = NULL;
	buffer_desc.flags = 0;
	buffer_desc.size = size;
	buffer_desc.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	buffer_desc.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	buffer_desc.queueFamilyIndexCount = 0;
	buffer_desc.pQueueFamilyIndices = NULL;
	VK_CHECK(vkCreateBuffer(vk.device, &buffer_desc, NULL, &vk_world.staging_buffer));

	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(vk.device, vk_world.staging_buffer, &memory_requirements);

	uint32_t memory_type = find_memory_type(vk.physical_device, memory_requirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkMemoryAllocateInfo alloc_info;
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.pNext = NULL;
	alloc_info.allocationSize = memory_requirements.size;
	alloc_info.memoryTypeIndex = memory_type;
	VK_CHECK(vkAllocateMemory(vk.device, &alloc_info, NULL, &vk_world.staging_buffer_memory));
	VK_CHECK(vkBindBufferMemory(vk.device, vk_world.staging_buffer, vk_world.staging_buffer_memory, 0));

	void* data;
	VK_CHECK(vkMapMemory(vk.device, vk_world.staging_buffer_memory, 0, VK_WHOLE_SIZE, 0, &data));
	vk_world.staging_buffer_ptr = (byte*)data;
}

