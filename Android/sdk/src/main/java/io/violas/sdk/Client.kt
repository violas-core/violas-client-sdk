package io.violas.sdk

class Client {

    external fun stringFromJNI(): String

    external fun createNativeClient(): Int

    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}
