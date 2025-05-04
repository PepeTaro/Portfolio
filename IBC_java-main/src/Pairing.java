/**
 * 参照 An Introduction to Mathematical Cryptography p343
 */
import java.math.BigInteger;

public class Pairing{
    
    /**
     * miller()内で使用するサブルーチン。
     * 上記の参考文献内のTheorem6.41に定義がある
     */
    public static Fq2 gPQ(EC<Fq2> ec,Point<Fq2> p,Point<Fq2> q,Point<Fq2> r){
	Fq2 l;
	
	if(p.x.equals(q.x) && p.y.equals(q.y)) // Fq2angent Point
	    if(p.y.isZero()) // Vertical
		return r.x.subtract(p.x);
	    else          // Non-Vertical Fq2angent
		l = (p.x.pow(2).multiply(3).add(ec.a)).divide(p.y.multiply(2));	
	else if(p.x.equals(q.x)) // Vertical
	    return r.x.subtract(p.x);
	else             // Non-Vertical
	    //l = (q.y - p.y)/(q.x - p.x)
	    l = (q.y.subtract(p.y)).divide(q.x.subtract(p.x));
		
	Fq2 numer = r.y.subtract(p.y).subtract(l.multiply(r.x.subtract(p.x)));
	Fq2 denom = r.x.add(p.x).add(q.x).subtract(l.square());
	return numer.divide(denom);
    }

    /**
     * Miller's algorithmを使用して、f(r)の値を計算して返す、
     * ここでfは、div(f) = m[p] - [mp] - (m-1)[INF]を満たす関数。
     */
    public static Fq2 miller(EC<Fq2> ec,Point<Fq2> p,BigInteger m,Point<Fq2> r){
	String binary  = m.toString(2);
	int bit_length = binary.length();

	Point<Fq2> t = new Point<Fq2>(p);
	Fq2 f = Fq2.getOne();
	
	for(int i=1;i < bit_length;i++){
	    f = (f.square()).multiply(gPQ(ec,t,t,r));
	    t = ec.add(t,t);
	    if(binary.charAt(i) == '1'){
		f = f.multiply(gPQ(ec,t,p,r));
		t = ec.add(t,p);
	    }
	}
	return f;	
    }

    /**
     * Weil Pairing e_{m}(p,q)を計算、点sは{INF,p,-q,p-q}に含まれない点。
     */
    public static Fq2 weilPairing(EC<Fq2> ec,Point<Fq2> p,Point<Fq2> q,Point<Fq2> s,BigInteger m){
	Point<Fq2> qs = ec.add(q,s);
	Point<Fq2> ps = ec.subtract(p,s);
	Point<Fq2> negS = ec.negate(s);

	Fq2 fpqs = miller(ec,p,m,qs);
	Fq2 fps  = miller(ec,p,m,s);

	Fq2 fqps   = miller(ec,q,m,ps);
	Fq2 fqNegs = miller(ec,q,m,negS);

	Fq2 numer = fpqs.divide(fps);
	Fq2 denom = fqps.divide(fqNegs);

	return numer.divide(denom);
    }
    
}
