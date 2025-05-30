from gmssl import sm2

# 生成 SM2 密钥对
private_key, public_key = sm2.generate_keypair()

print(f"Private Key: {private_key}")
print(f"Public Key: {public_key}")
