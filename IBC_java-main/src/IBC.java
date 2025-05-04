import java.math.BigInteger;
import java.util.Random;
import java.util.Arrays;
import java.util.List;

import java.io.Serializable;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

/**
 * IBC関連(暗号化や復号化など)メソッドを格納したクラス
 */
public class IBC{
    
    private static void assertTrue(boolean test,String message){
	if(! test)
	    throw new RuntimeException(message);
    }
        
    /**
     *  lower <= n <= upperを満たす整数乱数nを返す
     */
    private static BigInteger randomBetween(BigInteger lower,BigInteger upper){
	BigInteger random;
	Random randomSource = new Random();
	
	do{
	    random = new BigInteger(upper.bitLength(),randomSource);
	}while(random.compareTo(lower) < 0);

	return random;
    }

    /**
     * 文字列sの先頭にpadLength個の"0"を追加する
     */
    private static String padZero(String s,int padLength){
	for(int i=0;i<padLength;i++) s = "0" + s;
	return s;
    }

    /**
     * 2進数文字列s1とs2のxorを計算し、その結果を文字列で返す。
     */ 
    public static String xor(String s1,String s2){
	assertTrue(s1.length() == s2.length(),"IBC.xor(): s1とs2の長さが異なります");

	String s = "";	
	for(int i=0;i<s1.length();i++){
	    if(s1.charAt(i) == s2.charAt(i))
		s += "0";
	    else
		s += "1";
	}

	return s;
    }

    /**
     * bytesを16進法文字列に変換
     */
    private static String toHex(byte[] bytes) {
        BigInteger bi = new BigInteger(1,bytes);
        return String.format("%0" + (bytes.length << 1) + "X", bi);
    }

    /**
     * 文字列をハッシュ、その結果を整数に変換して返す。
     */
    private static BigInteger hashStringIntoBigInteger(String s){
        try{
            MessageDigest messageDigest = MessageDigest.getInstance("SHA-256");
            messageDigest.update(s.getBytes());
            String hex = toHex(messageDigest.digest());
            return new BigInteger(hex,16);

        } catch(NoSuchAlgorithmException e){
            throw new RuntimeException("IBC.hashStringIntoBigInteger(): ハッシュエラー");
        }

    }

    /**
     * 整数yの三乗根をFq上で計算
     */
    public static BigInteger computeCubeRoot(BigInteger y,BigInteger q){
	assertTrue(q.mod(new BigInteger("3")).equals(new BigInteger("2")),"IBC.computeCubeRoot(): q%3 == 2");
	
	// (y*y - 1)^((2*q - 1)/3) mod q
	BigInteger base = (y.multiply(y)).subtract(BigInteger.ONE); // y*y - 1
        BigInteger exponent = (q.multiply(new BigInteger("2")).subtract(BigInteger.ONE)).divide(new BigInteger("3")); // (2*q - 1)/3     
        return base.modPow(exponent,q);
    }

    /**
     * idをハッシュする関数、文字列は楕円曲線ec上の点。
     */
    public static Point<Fq2> hash1(EC<Fq2> ec,String id){
	BigInteger q = ec.getPrime();
	BigInteger y = hashStringIntoBigInteger(id).mod(q);	
	BigInteger x = computeCubeRoot(y,q);

	Point<Fq2> p = new Point<Fq2>(new Fq2(x),new Fq2(y));
	Point<Fq2> p6 = ec.multiply(6,p);

	assertTrue(ec.isOnCurve(p),"IBC.hash1(): 点が楕円曲線上にない");	
	assertTrue(! p6.isINF(),"IBC.hash1(): ハッシュの生成に失敗しました、IDを変えてください");

	return p6;	
    }

    /**
     * Fq2の点pを、長さがnである2進数文字列に変換。
     *
     * hash2(Fq2(x,y)) := Tr(Fq2(x,y)) + q*y、
     * として定義する。
     * ここでTrはFq2上のTrace Map。
     *
     * この定義を使用する理由は、H2は単射であるため、つまりCollision Free。
     * Tr(Fq2(x,y))がFqの元つまり、{0,1,...,q-1}の内どれかであることに
     * 注意すれば、単射であることの証明は自明。
    */
    public static String hash2(Fq2 p,int n){
        Fq q = new Fq(Field.getPrime());
        Fq t = p.trace();
        Fq x = t.add(q.multiply(p.y));
	String s  = (x.n).toString(2);
	
	int length = s.length();
	if(length > n){
	    return s.substring(0,n);
	}else{
	    s = padZero(s,n - length);
	    return s;
	}
    }

    /**
     * 楕円曲線ec上のランダムな点を返す
     */
    public static Point<Fq2> findRandomPoint(EC<Fq2> ec){
	BigInteger q = ec.getPrime();
	
	// y = randomBetween(0,q-1)
	BigInteger y = randomBetween(BigInteger.ZERO,q.subtract(BigInteger.ONE));
	BigInteger x = computeCubeRoot(y,q);

	Point<Fq2> p = new Point<Fq2>(new Fq2(x),new Fq2(y));
	assertTrue(ec.isOnCurve(p),"IBC.gindRandomPoint(): 点が楕円曲線上にない");
	
	return p;		
    }

    /**
     * 引数orderで指定された位数をもつ楕円曲線上の点を返す
     */
    public static Point<Fq2> findPointWithOrder(EC<Fq2> ec,BigInteger order){
	Point<Fq2> p,p6;
	while(true){
	    p = findRandomPoint(ec);
	    p6 = ec.multiply(6,p);
	    
	    if(! p6.isINF())
		break;	    
	}

	assertTrue(ec.multiply(order,p6).isINF(),"IBC.findPointWithOrder(): l*p6 == INF");
	return p6;
    }

    /**
     * Distortion Mapを計算して返す,Point(x,y) => (omega*x,y)
     * ここで、omagaはFq2上の非自明な1の三乗根
     */
    public static Point<Fq2> phi(EC<Fq2> ec,Point<Fq2> p){
        final Fq2 omega = new Fq2(BigInteger.ONE,BigInteger.ZERO);
	assertTrue(omega.pow(3).equals(Fq2.getOne()),"IBC.phi(): omegaエラー");

	Point<Fq2> result = new Point<Fq2>(p.x.multiply(omega),p.y);
	return result;
    }

    /**
     * Modified Weil Pairingを計算
     */
    public static Fq2 modifiedWeilPairing(EC<Fq2> ec,Point<Fq2> p1,Point<Fq2> p2,BigInteger order){
	Point<Fq2> s = findRandomPoint(ec);
	return Pairing.weilPairing(ec,p1,phi(ec,p2),s,order);
    }

    /**
     *  楕円曲線、楕円曲線上の点、その点の位数を返す。
     *  @return EC<Fq2>,Point<Fq2>,BigInteger
     */
    public static List<Object> generateEC(int securityBit){
	if(securityBit < 20){
	    throw new RuntimeException("IBC.generateEC(): セキュリティビットが低すぎます");
	}
	
        List<Object> params = generateParams(securityBit);
	BigInteger q = (BigInteger)params.get(0);
	BigInteger order = (BigInteger)params.get(1);
	
	Fq2.setPrime(q);
	EC<Fq2> ec = new EC<Fq2>(new Fq2("0"),new Fq2("1"));	
	Point<Fq2> p = findPointWithOrder(ec,order);
	return Arrays.asList(ec,p,order);
    }

    /**
     * 楕円曲線の生成に必要なパラメータ(q,l)を返す、ここでq,lはともに素数でq = 6*l - 1を満たす
     * @return BigInteger,BigInteger
     */
    public static List<Object> generateParams(int securityBit){
	Random randomSource = new Random();
        int maxIter = 2147483647; // intの最大値
	int certainty = 100;
	BigInteger q,l;
	
	for(int i=0;i<maxIter;i++){
	    l = BigInteger.probablePrime(securityBit,randomSource); // lは素数
	    q = l.multiply(new BigInteger("6")).subtract(BigInteger.ONE); // 6*l - 1.

	    // 素数を見つけた場合
	    if(q.isProbablePrime(certainty)){
		return Arrays.asList(q,l);
	    }
	}

	throw new RuntimeException("IBC.generateParams(): パラメータ生成に失敗");
    }

    /**
     * 秘密鍵生成時に必要なシークレットキーと公開鍵(楕円曲線上の点)を返す
     * @return BigInteger,Point<Fq2>
     */
    public static List<Object> generatePublicKeyParams(EC<Fq2> ec,Point<Fq2> p,BigInteger order){
	BigInteger secretRandom = randomBetween(BigInteger.ONE,order.subtract(BigInteger.ONE));
	Point<Fq2> publicKey = ec.multiply(secretRandom,p);
	return Arrays.asList(secretRandom,publicKey);
    }

    /**
     * idをハッシュした点と秘密鍵を返す
     * @return Point<Fq2>,Point<Fq2>
     */
    public static List<Object> generatePrivateKeyParams(EC<Fq2> ec,BigInteger secretRandom,String id){
	Point<Fq2> h1 = hash1(ec,id);
	Point<Fq2> privateKey = ec.multiply(secretRandom,h1);
	return Arrays.asList(h1,privateKey);
    }

    /**
     *  文字列mを暗号化する、"暗号文"はタプル(c1,c2)
     *  @param m: 暗号化する文字列(2進数文字列である必要あり)
     *  @param n: mの長さ(ビット長)
     *  @return: Point<Fq2>,String
     */
    public static List<Object> encrypt(EC<Fq2> ec,String m,Point<Fq2> publicKey,Point<Fq2> h1,Point<Fq2> p,BigInteger order,int n){
	BigInteger r = randomBetween(BigInteger.ONE,order.subtract(BigInteger.ONE));
	Fq2 gID = modifiedWeilPairing(ec,h1,publicKey,order);

	Point<Fq2> c1 = ec.multiply(r,p);
	String c2 = xor(m,hash2(gID.pow(r),n));
	
	return Arrays.asList(c1,c2);
    }

    /**
     * (c1,c2)を復号化
     * @param n: 平文を2進数文字列で表したときの長さ(ecnrypto()の引数nと同じ値)
     * @return String
     */
    public static String decrypt(EC<Fq2> ec,Point<Fq2> c1,String c2,Point<Fq2> privateKey,BigInteger order,int n){
	try{
	    Fq2 w = modifiedWeilPairing(ec,privateKey,c1,order);
	    String m = xor(c2,hash2(w,n));
	    return m;	
	}catch(Exception e){
	    throw new RuntimeException("IBC.decrypt(): 復号化に失敗しました。セキュリティビットを上げて、もう一度暗号化から試してください");
	}
    }
    
}
