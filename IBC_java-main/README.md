# Identity-based cryptography(IBC) デモ

## 特徴
* y^2 = x^3 + 1の形をした楕円曲線を使用。
* PairingはModified Weil pairingを使用。
* ./pdf/theory.pdf内に理論的な説明あり

## 注意
* 32ビット Debian GNU/Linux 11 (bullseye)上でのみ動作確認済み。
* セキュリティが低く(疑似乱数すら使用している)、かつ最適化も完全にしていない。
* 確率的にWeil pairingが失敗する(バグではなく、アルゴリズムの仕様)。

### デモ
```java
int securityBit = 20; // この値が高いほどセキュリティが高くなる(低すぎると、アルゴリズムの仕様上、復号化に失敗する)
String plainText = "111111111111111111111111111111"; // 平文
int n = plainText.length(); 

// ec = 楕円曲線、p = 楕円曲線上の点、order = 点pの位数
List<Object> ecParams = IBC.generateEC(securityBit);
EC<Fq2> ec = (EC<Fq2>)ecParams.get(0);
Point<Fq2> p = (Point<Fq2>)ecParams.get(1);
BigInteger order = (BigInteger)ecParams.get(2);

// s = 秘密にしておく乱数、pubkey = 公開鍵
List<Object> pubkeyParams = IBC.generatePublicKeyParams(ec,p,order);
BigInteger s = (BigInteger)pubkeyParams.get(0);
Point<Fq2> pubkey = (Point<Fq2>)pubkeyParams.get(1);

// 秘密鍵生成に使用するID
String id = "Alice@gmail.com";

// h1 = 暗号化に必要な点、privkey = 秘密鍵
List<Object> privkeyParams = IBC.generatePrivateKeyParams(ec,s,id);
Point<Fq2> h1 = (Point<Fq2>)privkeyParams.get(0);
Point<Fq2> privkey = (Point<Fq2>)privkeyParams.get(1);

// (c1,c2) = 暗号文
List<Object> encData = IBC.encrypt(ec,plainText,pubkey,h1,p,order,n);
Point<Fq2> c1 = (Point<Fq2>)encData.get(0);
String c2 = (String)encData.get(1);

// decText = 復号文
String decText = IBC.decrypt(ec,c1,c2,privkey,order,n);

// 平文 = 復号 = "111111111111111111111111111111"
System.out.printf("平文:   %s\n",plainText);
System.out.printf("復号文: %s\n",decText);

System.out.printf("復号化に成功? %b\n",plainText.equals(decText)); 
```

### テスト
```sh
cd ./tests
bash ./test.sh
```

#### 参考文献
* Lawrence C. Washington. Elliptic Curves Number Theory and Cryptography, Second Edition
* J.H. Silverman, Jill Pipher, Jeffrey Hoffstein. An Introduction to Mathematical Cryptography
* Craig Costello.Pairings for beginners. https://static1.squarespace.com/static/5fdbb09f31d71c1227082339/t/5ff394720493bd28278889c6/1609798774687/PairingsForBeginners.pdf
* Ben Lynn.ON THE IMPLEMENTATION OF PAIRING-BASED CRYPTOSYSTEMS. https://crypto.stanford.edu/pbc/thesis.pdf
* R. W. Mak.Identity-based encryption using supersingular curves with the Weil pairing. https://fse.studenttheses.ub.rug.nl/12902/1/Thesis-1.pdf
