#include "LVE_Descriptors.h"
// std
#include <cassert>
#include <stdexcept>

namespace LVE {

    // *************** Descriptor Set Layout Builder *********************

    LVE_DescriptorSetLayout::Builder& LVE_DescriptorSetLayout::Builder::addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count) {
        assert(_bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        _bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<LVE_DescriptorSetLayout> LVE_DescriptorSetLayout::Builder::build() const {
        return std::make_unique<LVE_DescriptorSetLayout>(_lveDevice, _bindings);
    }

    // *************** Descriptor Set Layout *********************

    LVE_DescriptorSetLayout::LVE_DescriptorSetLayout(
        LVE_Device& lveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
        : _lveDevice{ lveDevice }, _bindings{ bindings } {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : _bindings) {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(
            _lveDevice.device(),
            &descriptorSetLayoutInfo,
            nullptr,
            &_descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    LVE_DescriptorSetLayout::~LVE_DescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(_lveDevice.device(), _descriptorSetLayout, nullptr);
    }

    // *************** Descriptor Pool Builder *********************

    LVE_DescriptorPool::Builder& LVE_DescriptorPool::Builder::addPoolSize(
        VkDescriptorType descriptorType, uint32_t count) {
        _poolSizes.push_back({ descriptorType, count });
        return *this;
    }

    LVE_DescriptorPool::Builder& LVE_DescriptorPool::Builder::setPoolFlags(
        VkDescriptorPoolCreateFlags flags) {
        _poolFlags = flags;
        return *this;
    }
    LVE_DescriptorPool::Builder& LVE_DescriptorPool::Builder::setMaxSets(uint32_t count) {
        _maxSets = count;
        return *this;
    }

    std::unique_ptr<LVE_DescriptorPool> LVE_DescriptorPool::Builder::build() const {
        return std::make_unique<LVE_DescriptorPool>(_lveDevice, _maxSets, _poolFlags, _poolSizes);
    }

    // *************** Descriptor Pool *********************

    LVE_DescriptorPool::LVE_DescriptorPool(
        LVE_Device& lveDevice,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes)
        : _lveDevice{ lveDevice } {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(_lveDevice.device(), &descriptorPoolInfo, nullptr, &_descriptorPool) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    LVE_DescriptorPool::~LVE_DescriptorPool() {
        vkDestroyDescriptorPool(_lveDevice.device(), _descriptorPool, nullptr);
    }

    bool LVE_DescriptorPool::allocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = _descriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(_lveDevice.device(), &allocInfo, &descriptor) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    void LVE_DescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const {
        vkFreeDescriptorSets(
            _lveDevice.device(),
            _descriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
    }

    void LVE_DescriptorPool::resetPool() {
        vkResetDescriptorPool(_lveDevice.device(), _descriptorPool, 0);
    }

    // *************** Descriptor Writer *********************

    LVE_DescriptorWriter::LVE_DescriptorWriter(LVE_DescriptorSetLayout& setLayout, LVE_DescriptorPool& pool)
        : _setLayout{ setLayout }, _pool{ pool } {}

    LVE_DescriptorWriter& LVE_DescriptorWriter::writeBuffer(
        uint32_t binding, VkDescriptorBufferInfo* bufferInfo) {
        assert(_setLayout._bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = _setLayout._bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        _writes.push_back(write);
        return *this;
    }

    LVE_DescriptorWriter& LVE_DescriptorWriter::writeImage(
        uint32_t binding, VkDescriptorImageInfo* imageInfo) {
        assert(_setLayout._bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto& bindingDescription = _setLayout._bindings[binding];

        assert(
            bindingDescription.descriptorCount == 1 &&
            "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        _writes.push_back(write);
        return *this;
    }

    bool LVE_DescriptorWriter::build(VkDescriptorSet& set) {
        bool success = _pool.allocateDescriptor(_setLayout.getDescriptorSetLayout(), set);
        if (!success) {
            return false;
        }
        overwrite(set);
        return true;
    }

    void LVE_DescriptorWriter::overwrite(VkDescriptorSet& set) {
        for (auto& write : _writes) {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(_pool._lveDevice.device(), _writes.size(), _writes.data(), 0, nullptr);
    }

}  // namespace lve