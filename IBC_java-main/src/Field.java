import java.math.BigInteger;

public abstract class Field<T>{
    protected static BigInteger q = null; // 素数
    
    public Field(){
	if(Field.q == null){
	    throw new RuntimeException("Field.Field(): 先にsetPrime()を使用して素数を設定する必要あり");
	}	
    }

    /**
     * 体に素数を設定
     */    
    public static void setPrime(BigInteger prime){
	int certainty = 100;
	if(! prime.isProbablePrime(certainty)){
	    throw new RuntimeException("Field.Field(): 引数は素数");
	}
        q = prime;
    }

    public static void setPrime(String prime){
	setPrime(new BigInteger(prime));
    }

    public static BigInteger getPrime(){
	return q;
    }
    
    public abstract T add (T a);                     // 加法
    public abstract T subtract(T a);                 // 減法
    public abstract T multiply(T a);                 // 乗法
    public abstract T multiply(long a);              // 乗法
    public abstract T divide  (T a);                 // 除法
    public abstract T pow     (long exponent);       // べき乗
    public abstract T pow     (BigInteger exponent); // べき乗
    public abstract T square  ();                    // 二乗
    public abstract T inverse ();                    // 乗法の逆元
    public abstract T negate  ();                    // 加法の逆元
    public abstract Fq trace   ();                   // Trace Map
    public abstract boolean equals(Object obj);      // objと等しいかチェック
    public abstract boolean isZero();                // 加法の単位元かチェック
    public abstract boolean isOne();                 // 乗法の単位元チェック
}
