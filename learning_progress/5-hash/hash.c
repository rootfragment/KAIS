#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<crypto/sha2.h>

MODULE_LICENSE("GPL");

static int __init hash_entry(void){
	int i;
	struct sha256_state sctx;
	u8 digest[SHA256_DIGEST_SIZE];
	const char *data1 = "hello";
	sha256_init(&sctx);
	
	sha256_update(&sctx, data1, strlen(data1));
	
	sha256_final(&sctx, digest);
	for(i=0;i<32;i++){
		pr_cont("%02x", digest[i]);
		}
	pr_cont("\n");
	return 0;
}

static void __exit hash_exit(void){
	pr_info("Exiting module");
}

module_init(hash_entry);
module_exit(hash_exit);
