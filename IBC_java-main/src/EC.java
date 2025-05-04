import java.math.BigInteger;

/**
 *  楕円曲線クラス。Tは楕円曲線の体。
 *  y^2 = x^3 + a*x + b.
 */
public class EC<T extends Field<T>>{
    // 楕円曲線の係数
    public T a;
    public T b;
    
    public EC(T a,T b){
	this.a = a;
	this.b = b;
	
	// 判別式 = 4*(this.a^3) + 27*(this.b^2)
	T disc = this.a.pow(3).multiply(4).add(this.b.pow(2).multiply(27));
	
	if(disc.isZero()){
	    throw new RuntimeException("EC.EC(): 判別式が0");
	}	
    }    

    /**
     * 楕円曲線の無限遠点を返す
     */
    public Point<T> INF(){
	return new Point<T>(null,null);
    }

    /**
     * 楕円曲線の体のモジュロを返す
     */
    public BigInteger getPrime(){
	return Field.getPrime();
    }

    /**
     * 点が楕円曲線上にある場合Trueを返す。
     */
    public boolean isOnCurve(Point<T> p){
	if(p.isINF())
	    return true;

	// (y^2 - x^3 - a*x - b)
	T test = p.y.square().subtract(p.x.pow(3)).subtract(this.a.multiply(p.x)).subtract(this.b);
	return test.isZero();
    }

    public Point<T> multiply(long n,Point<T> p){
	return multiply(BigInteger.valueOf(n),p);
    }

    /**
     * n*p := p + p + ... + p (n個)を計算して返す、ここでpは楕円曲線上の点。
     */
    public Point<T> multiply(BigInteger n,Point<T> p){
	if(p.isINF()) return INF();	
	if(n.equals(BigInteger.ZERO)) return INF();
	if(n.compareTo(BigInteger.ZERO) == -1) // n < 0
	    return multiply(n.negate(),negate(p));

	Point<T> q = p;
	Point<T> r = INF();
	while(n.compareTo(BigInteger.ZERO) == 1){ // n > 0
	    if(n.testBit(0)){ // nが奇数の場合
		r = add(r,q); // r = r + q
	    }
	    
	    q = add(q,q);	    
	    n = n.shiftRight(1); // n = n >> 1
	}

	return r;
    }

    /**
     * (-p)を計算、pは楕円曲線上の点。
     */
    public Point<T> negate(Point<T> p){
	if(p.isINF()) return INF();
	Point<T> minusP = new Point<T>(p.x,p.y.negate());
	return minusP;
    }

    /**
     *  p1 + p2 を計算、p1,p2は楕円曲線上の点。
     */
    public Point<T> add(Point<T> p1,Point<T> p2){
	T numer,denom,slope;
	
	if(p1.isINF())
	    return p2;
	else if(p2.isINF())
	    return p1;
	else if(p1.x.equals(p2.x) && p1.y.equals(p2.y.negate()))
	    return INF();
	else if(p1.x.equals(p2.x) && p1.y.equals(p2.y)){
	    numer = (p1.x.square().multiply(3)).add(this.a);
	    denom = p1.y.multiply(2);
	}else{
	    numer = p2.y.subtract(p1.y);
	    denom = p2.x.subtract(p1.x);
	}	          

	slope = numer.divide(denom);
	T x3 = slope.square().subtract(p1.x).subtract(p2.x);
	T y3 = slope.multiply(p1.x.subtract(x3)).subtract(p1.y);

	return new Point<T>(x3,y3);
    }

    
    /**
     *  p1 - p2 を計算、p1,p2は楕円曲線上の点。
     */  
    public Point<T> subtract(Point<T> p1,Point<T> p2){
	return add(p1,negate(p2));
    }
}
