from gmssl import sm3, sm2, sm4
import os

# 测试数据
content = b"ABC123"  # 字节串形式的内容

# 使用 SM3 生成哈希值
hash_value = sm3.sm3_hash(content)
print(f"SM3 Hash: {hash_value}")

# 替换为实际生成的公私钥
public_key = "your_actual_public_key"
private_key = "your_actual_private_key"

# 初始化 SM2 加密对象
sm2_crypt = sm2.CryptSM2(public_key=public_key, private_key=private_key)

# 使用 SM2 对哈希值进行签名
signature = sm2_crypt.sign(hash_value.encode(), private_key)
print(f"SM2 Signature: {signature.hex()}")

# 验证签名
verify_result = sm2_crypt.verify(signature, hash_value.encode(), public_key)
print(f"SM2 Verify Result: {verify_result}")

# 生成随机密钥
key = os.urandom(16)  # 16字节的密钥

# 初始化 SM4 加密对象
sm4_cipher = sm4.CryptSM4()

# 使用 SM4 对内容进行加密
encrypted_content = sm4_cipher.crypt_cbc(sm4.SM4_ENCRYPT, key, content)
print(f"SM4 Encrypted Content: {encrypted_content.hex()}")

# 使用 SM4 对加密内容进行解密
decrypted_content = sm4_cipher.crypt_cbc(sm4.SM4_DECRYPT, key, encrypted_content)
print(f"SM4 Decrypted Content: {decrypted_content}")
