
/**
 * 楕円曲線上の点を表すクラス
 */
public class Point<T extends Field>{
    // x座標とy座標
    public T x;
    public T y;

    public Point(T x,T y){
	this.x = x;
	this.y = y;
    }

    public Point(Point<T> p){
	this.x = p.x;
	this.y = p.y;
    }

    @Override
    public String toString(){
	String s;
	s  = "(" + this.x + "," + this.y + ") ";
	return s;
    }
    
    public boolean equals(Point<T> p){
	if(this.isINF() && ! p.isINF()) 
	    return false;
	else if(! this.isINF() && p.isINF())
	    return false;
	else if(this.isINF() && p.isINF())
	    return true;
	else
	    return this.x.equals(p.x) && this.y.equals(p.y);	
    }

    /**
     * 無限遠点である場合Trueを返す、無限点は(null,null)で表される。
     */
    public boolean isINF(){
	return(this.x == null && this.y == null);
    }
    
}
