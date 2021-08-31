script {
    use DiemFramework::DiemAccount;

    fun main() {
        DiemAccount::mine_vls();
    }
}