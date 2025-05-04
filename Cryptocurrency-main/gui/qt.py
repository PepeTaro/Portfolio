import sys

from PyQt5.QtWidgets import QApplication,QWidget,QLabel,QVBoxLayout,QGraphicsView,QGraphicsScene,QGraphicsItem,QGraphicsLineItem,QGraphicsRectItem,QGraphicsTextItem
from PyQt5.QtGui import QColor,QPainter,QPen,QGradient,QBrush,QVector2D,QTransform
from PyQt5.QtCore import Qt,pyqtSignal,QPointF,QRectF,qDebug

class GraphicsItem(QGraphicsItem):
    def __init__(self,item_type):
        super().__init__()
        self.item_type = item_type # アイテムの種類
        
    def transform(self,transform):
        raise NotImplementedError
    
class LineItem(GraphicsItem):
    def __init__(self,p1,p2):
        super().__init__("line")
        self.p1 = p1 # 線分の始点
        self.p2 = p2 # 線分の終点

        # パラメータ
        self._color = Qt.black # 線分の色
        self._width = 1        # 線分の太さ

    def boundingRect(self):        
        # self.p1.x() < self.p2.x() を満たすようにする
        if self.p1.x() >= self.p2.x():
            tmp = self.p1
            self.p1  = self.p2
            self.p2  = tmp
            
        if self.p1.y() < self.p2.y():
            return QRectF(self.p1.toPointF(),self.p2.toPointF()).normalized()
        else:
            return QRectF(self.p2.toPointF(),self.p1.toPointF()).normalized()
        
    def transform(self,transform):
        self.p1 = QVector2D(transform.map(self.p1.toPointF()))
        self.p2 = QVector2D(transform.map(self.p2.toPointF()))
        self.prepareGeometryChange()
        
    def paint(self,painter,option,widget):
        painter.setPen(QPen(self._color,self._width))
        painter.drawLine(self.p1.toPointF(),self.p2.toPointF())        
        
class RectItem(GraphicsItem):
    def __init__(self,p,width,height):
        super().__init__("rect")
        self.p = p
        self.width = width
        self.height = height
        
    def transform(self,transform):
        self.p = QVector2D(transform.map(self.p.toPointF()))
        
        # これがないとスクリーンを動いた後に、BoundingBoxをマウスクリックしたとき反応しない
        self.prepareGeometryChange()
        
    def boundingRect(self):
        return QRectF(self.p.x(),self.p.y(),self.width,self.height)
    
    def paint(self,painter,option,widget):
        painter.drawRect(self.boundingRect())

    def mousePressEvent(self,event):
        super().mousePressEvent(event)

class TextItem(GraphicsItem):
    _z_value = 1
    
    def __init__(self,p,width,height,text):
        super().__init__("text")
        self.p = p
        self.width = width
        self.height = height
        self.text = text        
        self.visible = False   # テキストが表示/非表示 状態かを表す
                
        # スクロールに必要な情報
        self.current_row = 0   # 現在の行
        self.idx_by_row = {}   # 行からself.messageのインデックスを得るために必要
        self.num_rows = None   # self.messageの行数                

        # パラメータ
        self._border_color = QColor(200,200,200,255) # Bouding Boxのボーダー色
        self._border_width = 5                       # Bouding Boxのボーダー太さ
        self._roudness     = 3                       # Bouding Boxの丸さ具合
        self._font_size    = 10.0                    # テキストのフォントサイズ
        self._font_color   = Qt.white                # テキストのフォント色
        self._scroll_step  = 5                      # ホイールするたびにスクロールする行数
        
        self.init()                
        self.setZValue(TextItem._z_value)
        
    def init(self):
        """ スクロール時に必要な情報を保存 """
        lines = self.text.split("\n")
        
        self.idx_by_row[0] = 0
        total_idx = 0
        for row,line in enumerate(lines):
            total_idx += len(line) + 1
            self.idx_by_row[row + 1] = total_idx
        
        self.num_rows = len(self.idx_by_row) # テキストの行数を保存
        
    def flip_visible(self):
        """ テキスト表示 <-> テキスト非表示の切り替え """
        
        if self.visible:
            self.visible = False            
            TextItem._z_value -= 1            
        else:
            self.visible = True
            TextItem._z_value += 1

        #最後に表示したテキストを一番上に来るようにする
        self.setZValue(TextItem._z_value)
        
    def wheelEvent(self,event):
        """ テキストをスクロール """
        
        if event.delta() > 0:
            if (self.current_row - self._scroll_step) >= 0:
                self.current_row -= self._scroll_step
        else:
            if (self.current_row + self._scroll_step) < self.num_rows:
                self.current_row += self._scroll_step
        
        super().wheelEvent(event)

    def transform(self,transform):
        self.p = QVector2D(transform.map(self.p.toPointF()))
        self.prepareGeometryChange()
        
    def boundingRect(self):        
        return QRectF(self.p.x(),self.p.y(),self.width,self.height)
        
    def paint(self,painter,option,widget):
        if not self.visible: # テキスト非表示の場合
            return

        # Bounding Boxを描写        
        painter.setPen(QPen(self._border_color,self._border_width));
        painter.setBrush(QBrush(QGradient(QGradient.PremiumDark))) # グラデーション
        painter.setRenderHint(QPainter.Antialiasing) # アンチエイリアス 
        painter.drawRoundedRect(self.boundingRect(),self._roudness,self._roudness)
        
        # Diagram.init()で座標系を切り替えたことでテキストが反転するため、元に戻している
        painter.translate(self.p.x(),self.p.y())
        painter.rotate(180.0)
        painter.scale(-1,1)
        painter.translate(-self.p.x(),-self.p.y())
        painter.translate(0,-self.height)

        # テキストを描写
        font = painter.font()
        font.setPointSize(self._font_size) # フォントサイズを変更
        painter.setFont(font)
        painter.setPen(self._font_color)
        painter.drawText(self.boundingRect(),self.get_text_from_current_row())
        
    def get_text_from_current_row(self):
        """ 現在の行(self.current_row)から最後の行までのテキストを返す """
        idx = self.idx_by_row[self.current_row]
        return self.text[idx:]

class BlockItem(RectItem):
    def __init__(self,p,width,height,text):
        super().__init__(p,width,height)
        self.item_type = "block"

        # パラメータ
        self._block_color_after_clicked = Qt.red # ブロックがクリックされた後に塗る色
        self._text_box_offset = QVector2D(self.width,0) # ブロックからこのベクトル分離れたところにテキストを表示
        self._text_box_pos    = self.p + self._text_box_offset # テキストの表示位置
        self._text_box_width  = 1200 # テキストのBounding Boxの横幅
        self._text_box_height = 500 # テキストのBounding Boxの縦幅
            
        self.text_item = TextItem(self._text_box_pos,self._text_box_width,self._text_box_height,text)
        
    def paint(self,painter,option,widget):
        if self.text_item.visible:
            painter.setBrush(self._block_color_after_clicked)            
        painter.drawRect(self.boundingRect())
        
    def mousePressEvent(self,event):
        self.text_item.flip_visible()
        super().mousePressEvent(event)
        
class DiagramView(QGraphicsView):
    def __init__(self,scene):
        super().__init__(scene)

        # スクロール機能をオフ
        self.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self.horizontalScrollBar().blockSignals(True)
        self.verticalScrollBar().blockSignals(True)

    def mousePressEvent(self,event):
        super().mousePressEvent(event)
        
class Diagram(QGraphicsScene):
    def __init__(self,width,height):        
        super().__init__()
        
        self.view  = DiagramView(self)
        self.items = [] # 描写するすべてのアイテムを格納
        self.prev_mouse_pos = None
        self.drag = False
        
        self.init(width,height)

    def wheelEvent(self,event):
        self.update()
        super().wheelEvent(event)
        
    def mousePressEvent(self,event):
        self.drag = True
        self.prev_mouse_pos = QVector2D(event.scenePos())
        self.update()
        
        super().mousePressEvent(event)
    
    def mouseReleaseEvent(self,event):
        self.drag = False
        super().mouseReleaseEvent(event)

    def mouseMoveEvent(self,event):
        # ドラッグ中の場合
        if self.drag is True:            
            scene_pos = QVector2D(event.scenePos())
            delta = scene_pos - self.prev_mouse_pos
            self.prev_mouse_pos = scene_pos        

            transform = QTransform().translate(delta.x(),delta.y())
            self.transform_all(transform)
            
        super().mouseMoveEvent(event)

    def init(self,width,height):
        # 左下の座標系に切り替え
        self.setSceneRect(0,0,width,height)
        self.view.scale(1,-1)

    def transform_all(self,transform):
        for item in self.items:
            item.transform(transform)            
        self.update()

    def add_item(self,item):
        # ブロックの場合テキストアイテムを含む
        if item.item_type == "block":
            self.items.append(item.text_item)
            self.addItem(item.text_item)
            
        self.items.append(item)
        self.addItem(item)
        
class MainWindow(QWidget):
    def __init__(self,width,height,title=""):
        assert width > 0 and height > 0
        super().__init__()
        
        self.width  = width
        self.height = height        
        self.diagram = Diagram(self.width,self.height)

        self.resize(self.width,self.height)
        self.setWindowTitle(title)
                
        usage_label = QLabel("使用方法: ブロックをクリックして、ブロックの内容を表示、もう一度押して閉じる。ウィンドウをドラッグすることで、'動き回れる'。テキストをスクロール可能。")
        buttons_label = QLabel("終了: Q")
        
        layout = QVBoxLayout()    
        layout.addWidget(usage_label)
        layout.addWidget(buttons_label)    
        layout.addWidget(self.diagram.view)
        
        self.setLayout(layout)

    def keyPressEvent(self,event):        
        key = event.key()        
        if key == Qt.Key_Q:
            QApplication.instance().quit()            
            
        super().keyPressEvent(event)
