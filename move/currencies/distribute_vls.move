script {
    use 0x1::LibraAccount;

    fun main() {
        LibraAccount::distribute_vls();
    }
}