#pragma once
#include "LVE_Device.h"
// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace LVE {

    class LVE_DescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(LVE_Device& lveDevice) : _lveDevice{ lveDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<LVE_DescriptorSetLayout> build() const;

        private:
            LVE_Device& _lveDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> _bindings{};
        };

        LVE_DescriptorSetLayout(LVE_Device& lveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~LVE_DescriptorSetLayout();
        LVE_DescriptorSetLayout(const LVE_DescriptorSetLayout&) = delete;
        LVE_DescriptorSetLayout& operator=(const LVE_DescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return _descriptorSetLayout; }

    private:
        LVE_Device& _lveDevice;
        VkDescriptorSetLayout _descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> _bindings;

        friend class LVE_DescriptorWriter;
    };

    class LVE_DescriptorPool {
    public:
        class Builder {
        public:
            Builder(LVE_Device& lveDevice) : _lveDevice{ lveDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<LVE_DescriptorPool> build() const;

        private:
            LVE_Device& _lveDevice;
            std::vector<VkDescriptorPoolSize> _poolSizes{};
            uint32_t _maxSets = 1000;
            VkDescriptorPoolCreateFlags _poolFlags = 0;
        };

        LVE_DescriptorPool(
            LVE_Device& lveDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~LVE_DescriptorPool();
        LVE_DescriptorPool(const LVE_DescriptorPool&) = delete;
        LVE_DescriptorPool& operator=(const LVE_DescriptorPool&) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        LVE_Device& _lveDevice;
        VkDescriptorPool _descriptorPool;

        friend class LVE_DescriptorWriter;
    };

    class LVE_DescriptorWriter {
    public:
        LVE_DescriptorWriter(LVE_DescriptorSetLayout& setLayout, LVE_DescriptorPool& pool);

        LVE_DescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        LVE_DescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        LVE_DescriptorSetLayout& _setLayout;
        LVE_DescriptorPool& _pool;
        std::vector<VkWriteDescriptorSet> _writes;
    };

}  // namespace LVE