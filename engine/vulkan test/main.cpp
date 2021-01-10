#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <set>

#include <cstdint> // max value of uint32_t

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME // availability of this extension is implied by availability of presentation queue
};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

// the unique createDebugUtilsMessenger fn must be loaded specific to the instance
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    // vkGetInstanceProcAddr returns pointer to fn of any vulkan command / fn. we cast it to the correct fn
    // it is vital that the quoted function name is accurate
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;// v
    std::optional<uint32_t> presentFamily; // these two may be the same queue for certain devices

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};


#include <fstream>
// binary file reader helper
static std::vector<char> readFile(const std::string& filename) {
    // ate - start from end of file - indicates filesize
    // binary - read as binary and do not make encoding transformations
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}


class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window; // manual destroy
    VkInstance instance; // manual destroy
    VkDebugUtilsMessengerEXT debugMessenger; // manual destroy
    VkSurfaceKHR surface; // manual destroy - Vulkan window interface extension surface
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; // implicit destruction with destruction of instance
    VkDevice device; // logical device to interface with the physical device - destroy manually
    VkQueue presentQueue; // queue to control presentation of rendered data (swap chain stuff)
    VkQueue graphicsQueue;
    VkSwapchainKHR swapChain; // destroy before logical device (created with logical device)
    std::vector<VkImage> swapChainImages; // automatically cleaned when the swap chain is destoryed
    VkFormat swapChainImageFormat; // image format we will chose for swapchain based on our preference
    VkExtent2D swapChainExtent; // swapchain image extent we will have selected based on the constraints
    std::vector<VkImageView> swapChainImageViews; // allocated and managed by us. destroy before destruction of logical device

    VkRenderPass renderPass; // must be destroyed before logical device destruction
    VkPipelineLayout pipelineLayout; // destroy at end of program before logical device
    VkPipeline graphicsPipeline;

    std::vector<VkFramebuffer> swapChainFramebuffers; // destroy after we finish all our drawing at end
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers; // in this implementation, the buffers are comprised of the same commands except for the one which binds drawing to the correct swapchain image at the correct time
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;


    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();

        createRenderPass();
        createGraphicsPipeline();

        createFramebuffers();
        createCommandPool();
        createCommandBuffers();
        createSemaphores();
    }

    void createInstance() {

        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers request, but not available!");
        }


        VkApplicationInfo appInfo{};
        // optional information to provide to Vulkan
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        // required
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        

        // validation layer and vk creation debug setup
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo; // link debug for vk instance creation
        } else {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }


        
        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();



        // retrieve list of all supported extensions
        uint32_t extensionCount = 0;
        // get amount of extensions
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        // create an appropriately sized object to record extensions
        std::vector<VkExtensionProperties> allExtensionsList(extensionCount);
        // call again, this time retrieving extension information
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, allExtensionsList.data());
        // print the extensions
        std::cout << "available extensions:\n";
        for (std::vector<VkExtensionProperties>::iterator it = allExtensionsList.begin(); it != allExtensionsList.end(); ++it) {
            std::cout << '\t' << it->extensionName << '\n';
        }



        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vulkan instance");
        }

    }

    //check if all the validationLayers specified by program above class are in the availableLayers
    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        
        //namely the Khronos validationLayer provided by LunarG

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }
        
        return true;
    }

    // return vector of required extensions, mainly glfw-required extensions and debug utils (if validation layers enabled)
    // such extensions include VK - window surface interface extension
    std::vector<const char*> getRequiredExtensions() {
        // check that this device has the Vulkan extensions to support GLFW
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount); // includes extensions such as VK_KHR_win32_surface, necessary for surface creation
    
        // instantiate extension return array with base of required glfwExtensions
        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        // append validation layer extension to extension requirements if debug mode enabled
        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }


    // setup runtime debug
    void setupDebugMessenger() {
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    // utilized twice, first to special create messenger for vulkan instance creation debugging
    // second for runtime debugging
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        // all (?) types enabled
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT; // specify all severities we would like to handle for
        // all types enabled
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // optional, this becomes avail again from pCallbackData in the callback
    
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            std::cerr << "severe warning - validation layer: " << pCallbackData->pMessage << std::endl;
        } else {
            std::cerr << "info - validation layer: " << pCallbackData->pMessage << std::endl;
        }

        return VK_FALSE;
    }

    // there are platform specific versions for this implementation which can conveniently be covered with glfw
    void createSurface() {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface));
    }


    void pickPhysicalDevice() {
        // first enumerate devices, record amount
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        // populate dynamic size array of devices with amount / info specified via vulkan fn
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        // check if we have a device that meets our requirements. choose the first such
        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }
        // if there are none, throw error.
        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    // extend as features are required
    bool isDeviceSuitable(VkPhysicalDevice device) {
        /*VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        printf(deviceProperties.deviceName);*/

        // do we have graphics / presentation queue families support on this device?
        QueueFamilyIndices indices = findQueueFamilies(device);


        // do we have swapchain extension support for image presentation
        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) { // swapChain capabilities can only be queried if there is swapChain support via extensions
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        //return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    // Find / verify support for required queue families
    // indeed, this function is actually called three times by the program so far, once in isDeviceSuitable, once for the logical device creation in createLogicalDevice() and once for swapchain creation to evaluate overlap of graphics / present queues
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        // query and then record queue support / availability for device
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        // cycle through the results and compare to hard-coded requirements based on eventual render usage
        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            // can the surface be presented to from the device?
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }
        return indices;
    }

    // compare vulkan detected extension support for devices to our required extension support
    bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        // our hard-coded requirements (e.g. swapchain extension)
        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        // erase requirements we support
        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        // any not supported, which are left?
        return requiredExtensions.empty();
    }


    void createLogicalDevice() {
        // first specify validated queues that we would like to enable
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        

        // populating queue create info structs with our queue requirements (currently graphical and present queue)
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
        // put required queue indices into a set to eliminate duplicates / overlap (e.g. potential graphics / present queue overlap)
        std::set<uint32_t> uniqueQueueFamilyIndices = {indices.graphicsFamily.value(), indices.presentFamily.value()};
        
        float queuePriority = 1.0f; // choose a value between 0 and 1 to influence execution scheduling
        for (uint32_t queueFamily : uniqueQueueFamilyIndices) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1; // request creation of just 1 queue per family (if they overlap, just one is created overall)
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(queueCreateInfo);
        }

        // specify set of device features that will be used in this struct - currently unused / default
        VkPhysicalDeviceFeatures deviceFeatures{};


        // main logical device creation struct
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        // link information to enable desired queues to logical existence
        createInfo.pQueueCreateInfos = queueCreateInfos.data(); // each queue create info has a pnext, in case more queues need to be specified
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

        createInfo.pEnabledFeatures = &deviceFeatures;

        
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        // create the logical device
        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        // attempt to retrieve a queue!
        // there's only one possible queue we requested that we can retrieve form the logical device for the presentation family
        // so the index is 0
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    }


    void createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        swapChainImageFormat = surfaceFormat.format; // store as class member for later use
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
        swapChainExtent = extent; // store as class member for later use


        // now create the swapchain after we have selected the parameters!
        // require one more image than the minimum to avoid deadlock on images
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        // do not exceed the maximum image count. !! - 0 means there is no maximum
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        
        // populate the struct for swapchain image creation
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace =  surfaceFormat.colorSpace;
        createInfo.presentMode = presentMode;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // images shared and transfer does not need to be specified between queue families specified - slower than explicit, below
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // images not shared and owndership must be explicitly transferred before operating from another queue
            createInfo.queueFamilyIndexCount = 0; // optional, not used, sharing mode exclusive
            createInfo.pQueueFamilyIndices = nullptr; // optional, not used
        }

        // specify things like rotate 90 degrees or flip
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

        // can specify to blend alpha with ambient window system! in this case, not chosen, though
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        // allow for performance increase by not caring about clipped pixels (by windows above)
        createInfo.clipped = VK_TRUE;

        // specified when recreating the swapchain for purposes like when the window is resized
        createInfo.oldSwapchain = VK_NULL_HANDLE;


        // create!
        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }


        // retrieve images
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
    }

    // query swapchain capabilities
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;

        // querying the details with our vulkan instance. the device and surface are main parameters to these functions as they are both vital to the swapchain
        // this function populates a struct which contains info such as min/max swap image size
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        // get available formats, regarding image colorspace / data format
        // as usual, query amount, then populate dynamically sized array of info structs
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        // get present modes in same fashion
        //
        uint32_t modesCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &modesCount, nullptr);

        if (modesCount != 0) {
            details.presentModes.resize(modesCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &modesCount, details.presentModes.data());
        }

        return details;
    }

    // after querying swapchain capabilities, choose those which best match our requirements

    // choose from previously queried available surfaces
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            // choose standard SRGB colorspace and 8 bits for R, G, B, A
            if (availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && availableFormat.format == VK_FORMAT_R8G8B8A8_SRGB) {
                return availableFormat;
            }
        }

        // if this format is not available, one can rank the others and choose the best available
        // in this case, just choose the first available and it should be fine
        return availableFormats[0];
    }

    // choose the presentation mode - in this case, triple buffering
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentationModes) {
        for (const auto& availablePresentationMode : availablePresentationModes) {
            // triple buffering
            if (availablePresentationMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentationMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    // can implement scaling here
    // set the swap chain images to the same size as the area we are rendering to
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        // if the previously queried surface capability extent member is the special value uint32 max
        // then we are given freedom to set this manually (to account for high DPI / scaling situations)
        if (capabilities.currentExtent.width != UINT32_MAX) {
            // otherwise vulkan has already specified this to be the same size as the window
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };
            
            // just choose biggest / smallest extent suitable in reference to actualExtent. no scaling implemented here 
            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    // create image views for each of the swap chain images. this defines/creates access to the images
    void createImageViews() {
        // resize default init class member to fit # of swapchain images
        swapChainImageViews.resize(swapChainImages.size());

        // populate image view create structs for each required swapchain image view
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // among options like 1D, 3D
            createInfo.format = swapChainImageFormat; // previously evaluated/queried best supported format
        
            // color swizzle mappings - identity chosen (default) - can otherwise move colors around
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            // no mipmaps or layers to swapchain images
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    
 
    void createRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat; // same as eventual target!
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // no multisampling yet
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // clear prev. frame
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // store into frame
    
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // not using yet

        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // other layouts optimize memory transfer and others ...
        // have image ready for swapchain presentation at end of pass


        // subpasses
        // all subsequently operate on same frame buffers for a pass
        // attachment reference is the way in which the data from the frame buffer is piped ?
        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        // ^ there's only one attachment as used in the fragment shader
        // other attachments include depthstencil attachments

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;


        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // refers to implicit subpass preceding (as value set for srcsubpass not dest)
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // wait on color attachment stage
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;


        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }


    void createGraphicsPipeline() {
        // programmable part of pipeline specification
        auto vertShaderCode = readFile("shaders/vert.spv");
        auto fragShaderCode = readFile("shaders/frag.spv");

        // lifetime = up to compilation/creation of graphics pipeline, then they can be destroyed
        // therefore they are local vars not class members
        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

        // create shader stages
        // other interesting attributes like pnext exist for this struct. check docs
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main"; // entrypoint - function to invoke
        // other useful attributes include specialization info, where one can specify
        // constant values used in the code, which the code is then optimized for (for example if there are if statements involving the constant)

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};



        // fixed function part pipeline specification

        // vertex data input format specification. currently, none
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;


        // input assembly
        // standard - triangles
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE; // (?) - allows for certain features


        // viewport -- casts/describes transformation from image to framebuffer
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        // scissor
        // draw to entire framebuffer - don't cut anything
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
        // populate struct
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;


        // rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE; // if enabled then fragments which are outside the depth of the view frustum are truncated to the frustum (outside near / far planes are clamped to the planes)
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // includes lines and points - lines can be used for wireframe debugging
        // ^ any mode other than fill, like lines, requires an extension
        rasterizer.lineWidth = 1.0f; // range availability based on physical device support
        // ^ widths other than 1 require extension
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // see other cull modes when typing VK_CULL_MODE
        // specify vertex face direction order
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        // unsure what the following 3 do yet
        rasterizer.depthBiasEnable = VK_FALSE; // sometimes used for shadow mapping
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;


        // multisampling - one way to antialias
        // disabled for now - to be revisited - no comments
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;


        // no depth / stencil testing yet, later for shadows


        // color blending
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT; // let all color pass through
        colorBlendAttachment.blendEnable = VK_FALSE; // cannot be true with other blending mode below at same time
        // this blending is disabled but if we were to implement it we could use the following
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // full weight to source, overwrite old swapchain buffer image entirely in this case
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // no weight given to image already there - not zero if this were a pass that goes after another for a single render cycle
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // you could draw light shafts with this, or bloom
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // may be able to use for something else

        // populate color blend state struct
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE; // this is if we were to use the other method of blending (bitwise combination)
        // !! if we were to toggle this ^ to be true then it disables the other blending mode
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment; // link other populated struct above
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;


        // Dynamic state - not much
        VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = 2;
        dynamicState.pDynamicStates = dynamicStates;


        // pipeline layout includes uniform values for shaders like transformations for rendering and instanced rendering
        // currently not used
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // not yet utilized
        pipelineLayoutInfo.pSetLayouts = nullptr; // ''
        pipelineLayoutInfo.pushConstantRangeCount = 0; // ''
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // ''

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }


        // put it all together!
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        // shader stage specs
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;

        // fixed function stage specs
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = nullptr; // optional

        pipelineInfo.layout = pipelineLayout;

        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;

        // look into pipeline compatibility

        // faster way to create pipelines reusing parts. must research
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }


        // THIS HAS TO HAPPEn AFTER PIPELInE CREATIOn
        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);

    }

    // VKShaderModule - light wrapper over shader bytecode for pipeline creation
    // generate shader module helper from bytecode
    VkShaderModule createShaderModule(const std::vector<char>& code) {
            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = code.size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

            VkShaderModule shaderModule;
            if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shader module!");
            }

            return shaderModule;
    }



    void createFramebuffers() {
        swapChainFramebuffers.resize(swapChainImageViews.size());

        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            VkImageView attachments[] = {
                swapChainImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }


    
    void createCommandPool() {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex - queueFamilyIndices.graphicsFamily.value(); // command buffers generated from command pools are submitted to one device queue
        poolInfo.flags = 0; // optional
        // one flag allows for inididual recreation of one command buffer without having to recreate all of them in the pool

        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {

            throw std::runtime_error("failed to create command pool!");
        }
    }


    void createCommandBuffers() {
        commandBuffers.resize(swapChainFramebuffers.size());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool; // allocate in the graphics family queue pool we previously created
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // check these details! pretty cool
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }


        // record commands into the buffers

        for (size_t i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0; // specifies flags for usage <- optimization. see flags
            beginInfo.pInheritanceInfo = nullptr;

            // begin writing commands to the buffer!
            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording to command buffer!");
            }


            // write command to begin specified render pass
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass; // the one and only so far
            renderPassInfo.framebuffer = swapChainFramebuffers[i];

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapChainExtent;

            VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

            vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

            vkCmdEndRenderPass(commandBuffers[i]);

            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }

    }


    void createSemaphores() {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS || vkCreateSemaphore(devicef, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS) {
            throw std::runtime_error("failed to create semaphores!");
        }
    }


    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            drawFrame();
        }
    }

    void drawFrame() {
        uint32_t imageIndex; // corresponds to swapchain image. used to signal which corresponding command buffer should be submitted
        // imageAvailableSemaphore signaled after image has finished being presented via its own queued command buffer preceding the current to be submitted
        vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}; // wait after vertex shader, before fragment / rasterization to image
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }


        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        presentInfo.pResults = nullptr;

        vkQueuePresentKHR(presentQueue, &presentInfo);
    }

    void cleanup() {

        // all commands finished and no more sync necessary (app over)
        vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);

        vkDestroyCommandPool(device, commandPool, nullptr);

        for (auto framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }

        vkDestroyPipeline(device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        vkDestroyRenderPass(device, renderPass, nullptr);

        // destroy self-allocated image views of swapchain images
        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(device, imageView, nullptr);
        }

        // destroy swapchain
        vkDestroySwapchainKHR(device, swapChain, nullptr);

        // destroy logical device
        vkDestroyDevice(device, nullptr);

        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        // destroy surface - must be destroyed before instance
        vkDestroySurfaceKHR(instance, surface, nullptr);

        // destroy instance, implicit destruction of physical device
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }
};


int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}