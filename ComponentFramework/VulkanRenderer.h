#ifndef VULKANRENDERER_H 
#define VULKANRENDERER_H

#include <SDL.h>
#include <SDL_vulkan.h>
#include <SDL_image.h>
#include <vulkan/vulkan.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <unordered_map>
#include <array>
#include <chrono>

#include "Vector.h"
#include "VMath.h"
#include "MMath.h"
#include "Hash.h"
using namespace MATH;


#include "Renderer.h"


const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG /// only use validation layers if in debug mode
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices {
    /// optional means that it contains no value until it is assigned
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

    struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

    struct Vertex {
        Vec3 pos;
        Vec3 normal;
        Vec2 texCoord;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
             
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, normal);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

            return attributeDescriptions;
        }
        bool operator == (const Vertex& other) const {
            return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
        }
        
    }; /// End of struct Vertex


    namespace std {
        template<> struct hash<Vertex> {
            size_t operator()(Vertex const& vertex) const noexcept {
                size_t hash1 = hash<Vec3>()(vertex.pos);
                size_t hash2 = hash<Vec3>()(vertex.normal);
                size_t hash3 = hash<Vec2>()(vertex.texCoord);
                size_t result = ((hash1 ^ (hash2 << 1)) >> 1) ^ (hash3 << 1);
                return result;
            }
        };
    }

    struct IndexedBufferMemory {
        VkBuffer vertBufferID;
        VkDeviceMemory vertBufferMemoryID;
        VkDeviceSize vertBufferSize;

        VkBuffer indexBufferID;
        VkDeviceMemory indexBufferMemoryID;
        VkDeviceSize indexBufferSize;

    };
struct CameraUBO {
    Matrix4 view;
    Matrix4 proj;
};

struct GLightsUBO {
    Vec4 position;
    Vec4 diffuse;
};
struct PushConstant {
    Matrix4 modelMatrix;
    Matrix4 normalMatrix;
};

struct Pipeline {
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipelineID;
};

struct Sampler {
    VkImage image;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
};

struct Model {
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
    IndexedBufferMemory buffer;
    Pipeline pipeline;
    PushConstant constants;
    Sampler sampler;
    int textureID;
};

class VulkanRenderer : public Renderer {
public:
    /// C11 precautions 
    VulkanRenderer(const VulkanRenderer&) = delete;  /// Copy constructor
    VulkanRenderer(VulkanRenderer&&) = delete;       /// Move constructor
    VulkanRenderer& operator=(const VulkanRenderer&) = delete; /// Copy operator
    VulkanRenderer& operator=(VulkanRenderer&&) = delete;      /// Move operator

    VulkanRenderer();
    ~VulkanRenderer();
    SDL_Window* CreateWindow(std::string name_, int width, int height);
    bool OnCreate();
    void OnDestroy();
    void Render();
    void SetCameraUBO(const Matrix4& projection, const Matrix4& view);
    void SetPushConstants(const Matrix4& model);
    void SetLightUBO(const std::vector<Vec4>& position, const std::vector<Vec4>& diffuse);
    SDL_Window* GetWindow() { return window; }
    void CreateTextureImage(VkImage& image_, VkDeviceMemory& imageMemory_, const char* filename);
    Pipeline CreateGraphicsPipeline(VkDescriptorSetLayout& descriptorSetLayout, const char* vertFile, const char* fragFile, const char* geomFile);
    void LoadModelIndexed(const char* filename, const char* vertFile, const char* fragFile, int textureID, const char* geomFile = nullptr);

    void switchDescriptors();

private:
    const size_t MAX_FRAMES_IN_FLIGHT = 2;
    SDL_Event sdlEvent;
    uint32_t windowWidth;
    uint32_t windowHeight;
    SDL_Window* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkRenderPass renderPass;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
    VkCommandPool commandPool;

    //handles to the 
    struct Buffer {
        VkBuffer bufferID;
        VkDeviceMemory memoryBuffer;
    };

    std::vector<Buffer> cameraBuffers;
    std::vector<Buffer> lightBuffers;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;

    bool framebufferResized = false;

    
    

    bool hasStencilComponent(VkFormat format);

    void initVulkan();
    void createInstance();
    void createSurface();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void recreateSwapChain();
    void updateCameraUniformBuffer(uint32_t currentImage);
    void updateLightUniformBuffer(uint32_t currentImage);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    void createRenderPass();
    void createDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout);
   
    void createFramebuffers();
    void createCommandPool();
    void createDepthResources();
   
    void createTextureImageView(VkImageView& textureImageView, VkImage& image_);
    void createTextureSampler(VkSampler& sampler);
    void createSamplers(std::vector<std::string> filenames);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
        VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
   
    void createVertexBuffer(IndexedBufferMemory& memoryBuffer, const std::vector<Vertex>& vertices);
        /// A helper function for createVertexBuffer()
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createIndexBuffer(IndexedBufferMemory& memoryBuffer, const std::vector<uint32_t>& indices);
    void createCameraUniformBuffers();
    void createLightUniformBuffers();
    void createDescriptorPool(VkDescriptorPool& descriptorPool);
    void createDescriptorSets(const VkDescriptorPool& descriptorPool, VkDescriptorSetLayout& descriptorSetLayout, std::vector<VkDescriptorSet>& descriptorSets, int textureID);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void createCommandBuffers();
    void recordCommandBuffers();
    void createSyncObjects();
    void cleanup();
    void cleanupSwapChain();
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    VkFormat findDepthFormat();
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    void  populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void setupDebugMessenger();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    void pickPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    
    CameraUBO cameraUBO;
    PushConstant constants;
    std::array<GLightsUBO, 3> lightUBO;
    std::vector<Model*> models;
    std::vector<Sampler> samplers;
    

    VkShaderModule createShaderModule(const std::vector<char>& code);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    static std::vector<char> readFile(const std::string& filename);

   
};
#endif 

