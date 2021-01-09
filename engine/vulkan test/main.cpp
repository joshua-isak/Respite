#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <set>

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
    VkDevice device; // logical device to interface with the physical device
    VkQueue presentQueue; // queue to control presentation of rendered data (swap chain stuff)

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



        /* retrieve list of all supported extensions
        uint32_t extensionCount = 0;
        // get amount of extensions
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        // create an appropriately sized object to record extensions
        std::vector<VkExtensionProperties> extensions(extensionCount);
        // call again, this time retrieving extension information
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        // print the extensions
        std::cout << "available extensions:\n";
        for (std::vector<VkExtensionProperties>::iterator it = extensions.begin(); it != extensions.end(); ++it) {
            std::cout << '\t' << it->extensionName << '\n';
        }*/



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

        //return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
        return indices.isComplete() && extensionsSupported;
    }

    // Find / verify support for required queue families
    // indeed, this function is actually called twice by the program so far, once in isDeviceSuitable and once for the logical device creation in createLogicalDevice()
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

        // our hard-coded requirements (swapchain extension)
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

        // device-specific validation layers are deprecated. they are only supported for instances, not logical devices. 0 enabled
        createInfo.enabledExtensionCount = 0;

        // create the logical device
        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        // attempt to retrieve a queue!
        // there's only one possible queue we requested that we can retrieve form the logical device for the presentation family
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
    }




    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
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