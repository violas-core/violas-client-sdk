script {
    use 0x1::LibraAccount;

    fun main() {
        LibraAccount::mine_vls();
    }
}