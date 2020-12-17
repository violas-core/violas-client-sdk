script {
    use 0x1::DiemAccount;

    fun main() {
        DiemAccount::mine_vls();
    }
}