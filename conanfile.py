from conans import ConanFile, CMake, tools

class TestDeviceApp(ConanFile):
    name = "TestDeviceApp"
    version = "0.1.0"
    generators = "cmake"
    default_user = "jenkins"
    default_channel = "master"


    def requirements(self):
        if self.user and self.channel:
            default_user = self.user
            default_channel = self.channel
        self.requires("coco/1.1.0@%s/%s" % (default_user, default_channel))


    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".")
        cmake.build()

