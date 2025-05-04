/**
 *  https://crypto.stanford.edu/pbc/thesis.pdf p77.
 */

import java.math.BigInteger;

/**
 * Fq2はFqの二次拡大体Fq2 = Fq[x]/(x^2 + x + 1)を表すクラス
 * q%3 == 2を満たす必要あり、そうでない場合、体にならない。(x^2 + x + 1が既約でない)
 */
class Fq2 extends Field<Fq2>{
    // Fq2の元は、タプル(x,y)で表される
    public Fq x; 
    public Fq y;
    
    public Fq2(BigInteger x,BigInteger y){
	super();
	checkPrime();
	
	this.x = new Fq(x);
	this.y = new Fq(y);
    }
    
    public Fq2(String x,String y){
	this(new BigInteger(x),new BigInteger(y));	
    }
    
    public Fq2(Fq x,Fq y){
        this(x.n,y.n);	
    }
    
    public Fq2(Fq2 fq2){
	this(fq2.x,fq2.y);
    }

    /**
     * 整数をFq2の元に変換,x => Fq2(-x,-x)    
     */
    public Fq2(BigInteger x){
	this(x.negate(),x.negate());
    }

    /**
     * 文字列で表される整数(e.g. "1","31415")をFq2の元に変換, x => Fq2(-Int(x),-Int(x))
     */
    public Fq2(String x){	
	this((new BigInteger(x)).negate(),(new BigInteger(x)).negate());	
    }

    @Override
    public String toString(){
	return "(" + this.x.n + "," + this.y.n + ") Fq2(mod " + Field.q + ")";
    }

    /**
     * q%3 == 2を満たすかチェック
     */
    private void checkPrime(){
	final BigInteger three = new BigInteger("3");
	final BigInteger two   = new BigInteger("2");
	if(! Field.q.mod(three).equals(two))
	    throw new RuntimeException("Fq2.checkPrime(): q%3 == 2");
    }

    public Fq2 add(Fq2 a){
        return new Fq2(this.x.add(a.x),this.y.add(a.y));
    }

    public Fq2 subtract(Fq2 a){
	return new Fq2(this.x.subtract(a.x),this.y.subtract(a.y));
    }

    /**
     *  a := (A,B),b = (C,D)とした場合、乗法は以下で定義される
     *  a*b := (B*D - A*D - B*C,A*C - A*D - B*C) 
     */
    public Fq2 multiply(Fq2 a){
	Fq bd = this.y.multiply(a.y); 
	Fq ad = this.x.multiply(a.y);
	Fq bc = this.y.multiply(a.x);
	Fq ac = this.x.multiply(a.x);
	Fq adbc = ad.add(bc);
	
	Fq c = bd.subtract(adbc);
	Fq d = ac.subtract(adbc);
        return new Fq2(c,d);
    }

    public Fq2 multiply(long a){
	BigInteger b = BigInteger.valueOf(a).negate();
	return this.multiply(new Fq2(b,b));
    }
    
    public Fq2 divide(Fq2 a){
	return this.multiply(a.inverse());
    }
    
    public Fq2 square(){
	final Fq two = new Fq("2");
	
	Fq a2 = two.multiply(this.x);
	Fq b2 = two.multiply(this.y);
	
	Fq c = this.y.subtract(a2);
	Fq d = this.x.subtract(b2);

	Fq e = this.y.multiply(c);
	Fq f = this.x.multiply(d);

        return new Fq2(e,f);
    }
    
    public Fq2 pow(BigInteger exponent){	
	String binary  = exponent.toString(2);
	int bit_length = binary.length();

	if(exponent.equals(BigInteger.ZERO)){ // this^0 = 1
	    return Fq2.getOne();
	}
	
        Fq2 r = new Fq2(this);
	for(int i=1;i < bit_length;i++){
	    r = r.square();
	    if(binary.charAt(i) == '1'){
		r = r.multiply(this);
	    }
	}
	return r;	
    }

    public Fq2 pow(long exponent){
	return this.pow(BigInteger.valueOf(exponent));
    }
    
    public Fq2 inverse(){
	if(this.isZero()){ // this.x == 0 and this.y == 0
	    throw new RuntimeException("Fq2.inverse(): 0で除算");
	    
	}else if(! this.x.isZero()){ // this.x != 0
	    Fq ba = this.y.divide(this.x);
	    Fq denom = this.y.multiply(ba);
	    denom = denom.add(this.x);
	    denom = denom.subtract(this.y);
	    Fq d = denom.inverse();
	    Fq c = ba.multiply(d);
	    return new Fq2(c,d);
	    
	}else{ // this.y != 0
	    Fq ab = this.x.divide(this.y);
	    Fq denom = this.x.multiply(ab);
	    denom = denom.add(this.y);
	    denom = denom.subtract(this.x);
	    Fq c = denom.inverse();
	    Fq d = ab.multiply(c);
	    return new Fq2(c,d);
	}
    }
    
    public Fq2 negate(){
	return new Fq2(this.x.negate(),this.y.negate());
    }

    /*
     * Trace Mapを計算。
     * Tr((a,b)) := (a,b) + (a,b)**q = (a,b) + (b,a) = (a+b,a+b) = -(a+b).
     * [注意] 戻り値の型はFqであることに注意。
    */    
    public Fq trace(){	    
        Fq t = this.x.add(this.y);
	return t.negate();
    }    

    public boolean equals(Object obj){
	if(obj.getClass() != this.getClass()) return false;	
        Fq2 a = (Fq2)obj;		
	return this.x.equals(a.x) && this.y.equals(a.y);
    }

    public boolean isZero(){
	return this.x.isZero() && this.y.isZero();
    }
    
    public boolean isOne(){
	BigInteger negOne = new BigInteger("-1");
	return this.x.equals(negOne) && this.y.equals(negOne);
    }
    
    public static Fq2 getZero(){	
        return new Fq2("0");    
    }

    public static Fq2 getOne(){	
        return new Fq2("1");    
    }

}
