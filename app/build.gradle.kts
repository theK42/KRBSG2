plugins {
    id("com.android.application")
}

android {
    namespace = "com.theK42.krbsg2"
    compileSdk = 33

    defaultConfig {
        applicationId = "com.theK42.krbsg2"
        minSdk = 33
        targetSdk = 33
        versionCode = 1
        versionName = "1.0"
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
    buildFeatures {
        prefab = true
    }

    sourceSets {
        getByName("main").java.srcDirs("src/main/java", "../KEngineBasics/SDL/android-project/app/src/main")
    }

    defaultConfig {
        // ...
        externalNativeBuild {
            cmake {
                arguments.addAll(setOf("-DBUILD_SHARED_LIBS=ON", "-DSDL2MIXER_OPUS=OFF", "-DCMAKE_ANDROID_ASSETS_DIRECTORIES=" + sourceSets.getByName("main").assets.srcDirs.first()))
            }
        }
        ndk {
            abiFilters += listOf("armeabi-v7a", "arm64-v8a")
        }
    }

    externalNativeBuild {
        cmake {
            path = file("../CMakeLists.txt")
            version = "3.22.1"
        }
    }
    ndkVersion = "25.2.9519653"
}

dependencies {

    implementation("androidx.appcompat:appcompat:1.6.1")
    implementation("com.google.android.material:material:1.8.0")
    //implementation("androidx.games:games-activity:1.2.2")
}