address 0x2 {
// a distributed key-value map is used to store entry (token_id, address, NonFungibleToken)
// key is the token_id(:vector<u8>), stored in a sorted linked list
// value is a struct 'NonFungibleToken', contains the non fungible token
// the account address of each list node is actually the owner of the token
module NonFungibleToken {
    use 0x2::Map;
    use 0x1::Option::{Option};
    use 0x1::Signer;
    use 0x1::Vector;
    use 0x1::BCS;
    use 0x1::Hash;

    const NFT_PUBLISHER: address = 0x2;

    struct LimitedMeta has key {
        limited: bool,
        total: u64,
    }

    struct NonFungibleToken<Token> has key, store {
        tokens: vector<Token>
    }

    struct TokenLock<Token> has key {
    }    

    fun lock<Token: store>(account: &signer) {
        move_to<TokenLock<Token>>(account, TokenLock<Token>{});
    }

    fun unlock<Token: store>(account: &signer) acquires TokenLock {
        let sender = Signer::address_of(account);
        let TokenLock<Token> {} = move_from<TokenLock<Token>>(sender);
    }

    public fun initialize<Token: store>(account: &signer, limited: bool, total: u64) {
        let sender = Signer::address_of(account);
        assert(sender == NFT_PUBLISHER, 8000);

        let limited_meta = LimitedMeta {
            limited: limited,
            total: total,
        };
        move_to<LimitedMeta>(account, limited_meta);
        
        //SimpleSortedLinkedList::create_new_list<vector<u8>>(account, Vector::empty());

        //SortedLinkedList::create_new_list<vector<u8>>(account, Vector::empty());

        Map::create<vector<u8>, address>(account);
    }
    //
    //  Get the number of balance for Token
    //
    public fun balance<Token : key + store>(sig: &signer) : u64 
    acquires NonFungibleToken {
        let sender = Signer::address_of(sig);

        let nft = borrow_global<NonFungibleToken<Token>>(sender);
        
        Vector::length<Token>(&nft.tokens)
    }
    //
    //
    //
    public fun make_token_id<Token>(token: &Token) : vector<u8> {
        let token_bcs = BCS::to_bytes<Token>(token);
        let token_id = Hash::sha3_256(token_bcs);

        token_id
    }
    //
    //  get token owner by token id
    //
    public fun get_token_owner(token_id: &vector<u8>): Option<address> {
        Map::get<vector<u8>, address>(NFT_PUBLISHER, token_id)
    }
    ///
    /// Mint a NFT to a receiver
    /// 
    public fun mint<Token: copy + drop + store>(sig: &signer, receiver: address, token: Token) : bool
    acquires NonFungibleToken  {
        let sender = Signer::address_of(sig);
        assert(sender == NFT_PUBLISHER, 1000);
        
        let token_id = make_token_id<Token>(&token);
                
        let ret = Map::insert(sender, token_id, receiver);
        if( !ret )  // token id has already existed.
            return false;        
        
        // The receiver must has called method 'accept_token'
        assert(exists<NonFungibleToken<Token>>(receiver), 1001);

        let receiver_token_ref_mut = borrow_global_mut<NonFungibleToken<Token>>(receiver);    
        
        Vector::push_back<Token>(&mut receiver_token_ref_mut.tokens, token);

        true
    } 

    public fun accept_token<Token: store>(account: &signer) {
        let sender = Signer::address_of(account);

        assert(!exists<NonFungibleToken<Token>>(sender), 8001);

        //SimpleSortedLinkedList::empty_node<vector<u8>>(account, Vector::empty());

        move_to<NonFungibleToken<Token>>(account, NonFungibleToken<Token>{tokens: Vector::empty<Token>()});
    }

    public fun safe_transfer<Token: drop + store>(sig: &signer, _token_id: vector<u8>, receiver: address) 
    acquires NonFungibleToken {
        let index = 0;
        
        transfer<Token>(sig, index, receiver);
    }

    public fun transfer<Token: drop + store>(account: &signer, index: u64, receiver: address) 
    acquires NonFungibleToken {
        let sender = Signer::address_of(account);

        assert(exists<NonFungibleToken<Token>>(receiver), 8002);                                
        assert(!exists<TokenLock<Token>>(sender), 8004);

        // Move NFT from sender to receiver
        // SimpleSortedLinkedList::move_node_to<vector<u8>>(copy account, receiver);

        let sender_token_ref_mut = borrow_global_mut<NonFungibleToken<Token>>(sender);
        let length = Vector::length<Token>(&sender_token_ref_mut.tokens);
        
        // Ensure the index is valid 
        assert(index < length, 8003);
        
        if (index < length - 1) 
            Vector::swap<Token>(&mut sender_token_ref_mut.tokens, index, length-1); //swap element to back

        let token = Vector::pop_back(&mut sender_token_ref_mut.tokens);

        let receiver_token_ref_mut =  borrow_global_mut<NonFungibleToken<Token>>(receiver);
        Vector::push_back<Token>(&mut receiver_token_ref_mut.tokens, token);

        // Move a Option::none to sender account
        //move_to<NonFungibleToken<Token>>(account, NonFungibleToken<Token>{token: Option::none()});
    }

       

    public fun get_nft<Token: store>(account: &signer): NonFungibleToken<Token> acquires NonFungibleToken {
        let sender = Signer::address_of(account);
        assert(exists<NonFungibleToken<Token>>(sender), 8006);
        assert(!exists<TokenLock<Token>>(sender), 8007);
        Self::lock<Token>(account);
        move_from<NonFungibleToken<Token>>(sender)
    }

    public fun put_nft<Token: store>(account: &signer, nft: NonFungibleToken<Token>) acquires TokenLock {
        let sender = Signer::address_of(account);
        assert(exists<TokenLock<Token>>(sender), 8008);
        Self::unlock<Token>(account);
        move_to<NonFungibleToken<Token>>(account, nft)
    }
}
}

/*
//! new-transaction
//! sender: nftservice
module TestNft {
    struct TestNft {}
    public fun new_test_nft(): TestNft {
        TestNft{}
    }
}
// check: "Keep(EXECUTED)"

//! new-transaction
//! sender: alice
// sample for moving Nft into another resource
module MoveNft {
    use {{nftservice}}::NonFungibleToken::{Self, NonFungibleToken};
    use {{nftservice}}::TestNft::TestNft;
    use 0x1::Signer;

    resource struct MoveNft {
        nft: NonFungibleToken<TestNft>
    }

    public fun move_nft(account: &signer) {
        let nft = NonFungibleToken::get_nft<TestNft>(account);
        move_to<MoveNft>(account, MoveNft{ nft });
    }

    public fun move_back_nft(account: &signer) acquires MoveNft {
        let sender = Signer::address_of(account);
        let MoveNft { nft } = move_from<MoveNft>(sender);
        NonFungibleToken::put_nft<TestNft>(account, nft);
    }
}
// check: "Keep(EXECUTED)"

//! new-transaction
//! sender: nftservice
script {
use {{nftservice}}::NonFungibleToken;
use {{nftservice}}::TestNft::TestNft;
fun main(account: signer) {
    NonFungibleToken::initialize<TestNft>(account, false, 0);
}
}

// check: "Keep(EXECUTED)"

//! new-transaction
//! sender: alice
script {
use {{nftservice}}::NonFungibleToken;
use {{nftservice}}::TestNft::{Self, TestNft};
use 0x1::Hash;
fun main(account: signer) {
    let input = b"input";
    let token_id = Hash::sha2_256(input);
    let token = TestNft::new_test_nft();
    NonFungibleToken::preemptive<TestNft>(account, {{nftservice}}, token_id, token);
}
}

// check: "Keep(EXECUTED)"

//! new-transaction
//! sender: alice
script {
use {{alice}}::MoveNft;
fun main(account: signer) {
    MoveNft::move_nft(account);
}
}

// check: "Keep(EXECUTED)"

//! new-transaction
//! sender: bob
script {
use {{nftservice}}::NonFungibleToken;
use {{nftservice}}::TestNft::TestNft;
fun main(account: signer) {
    NonFungibleToken::accept_token<TestNft>(account);
}
}

// check: "Keep(EXECUTED)"

//! new-transaction
//! sender: alice
script {
use {{nftservice}}::NonFungibleToken;
use {{nftservice}}::TestNft::TestNft;
use 0x1::Hash;
fun main(account: signer) {
    let input = b"input";
    let token_id = Hash::sha2_256(input);
    NonFungibleToken::safe_transfer<TestNft>(account, {{nftservice}}, token_id, {{bob}});
}
}

// check: ABORTED

//! new-transaction
//! sender: alice
script {
use {{alice}}::MoveNft;
fun main(account: signer) {
    MoveNft::move_back_nft(account);
}
}

// check: "Keep(EXECUTED)"

//! new-transaction
//! sender: alice
script {
use {{nftservice}}::NonFungibleToken;
use {{nftservice}}::TestNft::TestNft;
use 0x1::Hash;
fun main(account: signer) {
    let input = b"input";
    let token_id = Hash::sha2_256(input);
    NonFungibleToken::safe_transfer<TestNft>(account, {{nftservice}}, token_id, {{bob}});
}
}

// check: "Keep(EXECUTED)"
*/
