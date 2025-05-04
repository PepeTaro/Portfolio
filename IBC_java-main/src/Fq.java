import java.math.BigInteger;

/**
 * 体Fq = {0,1,2,...,q-1}を表すクラス、ここでqは素数
 */
class Fq extends Field<Fq>{
    // Fqの元
    public BigInteger n; // 0 <= n < q
    
    public Fq(BigInteger n){
	super();
	this.n = n.mod(Field.q);
    }

    public Fq(String n){
        this(new BigInteger(n));
    }

    public Fq(Fq fq){
        this(fq.n);
    }

    @Override
    public String toString(){
	return this.n + "(mod " + Field.q + ")";
    }

    public Fq add(Fq a){
	return new Fq(this.n.add(a.n).mod(Field.q));
    }

    public Fq subtract(Fq a){
	return new Fq(this.n.subtract(a.n).mod(Field.q));
    }

    public Fq multiply(long a){
	BigInteger n = BigInteger.valueOf(a);
	return new Fq(this.n.multiply(n).mod(Field.q));
    }

    public Fq multiply(Fq a){
	return new Fq(this.n.multiply(a.n).mod(Field.q));
    }
    
    public Fq divide(Fq a){
	return this.multiply(a.inverse());
    }

    public Fq square(){
	return this.multiply(this);
    }

    public Fq pow(long exponent){
	return pow(BigInteger.valueOf(exponent));
    }

    public Fq pow(BigInteger exponent){
	return new Fq(this.n.modPow(exponent,Field.q));
    }
    
    public Fq inverse(){
	if(this.equals(BigInteger.ZERO))
	    throw new IllegalArgumentException("Field.inverse(): 0の逆元は存在しない");
	return new Fq(this.n.modInverse(Field.q));
    }

    public Fq negate(){
	return new Fq(this.n.negate().mod(Field.q));
    }

    /**
     * Trace Mapの値を返す。
     */
    public Fq trace(){
	return new Fq(this.n);
    }

    public boolean equals(Object obj){	
	BigInteger diff;
	
	if(obj.getClass() == this.getClass()){
	    final Fq a = (Fq)obj;	
	    diff = this.n.subtract(a.n).mod(Field.q);
	}else if(obj instanceof BigInteger){
	    final BigInteger a = (BigInteger)obj;	
	    diff = this.n.subtract(a).mod(Field.q);
	}else{
	    return false;
	}
	
	if(diff.equals(BigInteger.ZERO))
	    return true;
	else
	    return false;
    }

    public boolean isZero(){
	return this.n.equals(BigInteger.ZERO);
    }

    public boolean isOne(){
	return this.n.equals(BigInteger.ONE);
    }

    public static Fq getZero(){	
        return new Fq("0");    
    }

    public static Fq getOne(){	
        return new Fq("1");    
    }

}
