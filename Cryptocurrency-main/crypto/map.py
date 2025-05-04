from typing import TypeVar,Generic
from crypto.json import json_dumps

T = TypeVar('T')

class Map(Generic[T]):
    """ dictのラッパー     """
    
    def __init__(self) -> None:        
        self._map = {} # dict[hash_val: str,obj: T]
        
    def get(self,hash_val: str) -> T:
        if not self.contains(hash_val):
            return None
        return self._map[hash_val]

    def keys(self) -> list[str]:
        return self._map.keys()

    def values(self) -> list[T]:
        return self._map.values()

    def size(self) -> int:
        return len(self._map)
                   
    def to_json(self) -> str:
        return json_dumps(self)
    
    def _count_hash(self,hash_val: str) -> int:
        """ 存在するなら1、しないなら0を返す """
        if hash_val not in self.keys():
            return 0
        else:
            return 1

    def _count_obj(self,obj: T) -> int:
        return self._count_hash(obj.get_hash())
    
    def count(self,arg) -> int:
        """ 引数がマップ内に含まれる場合1、そうでない場合0を返す """
        if isinstance(arg,str):
            return self._count_hash(arg)
        else: # type(arg) = T
            return self._count_obj(arg)
        
    def _contains_hash(self,hash_val: str) -> bool:
        return self.count(hash_val) == 1

    def _contains_obj(self,obj: T) -> bool:
        return self.count(obj) == 1
    
    def contains(self,arg) -> bool:
        """ 引数がマップ内に含まれる場合True、そうでない場合Falseを返す """
        if isinstance(arg,str):
            return self._contains_hash(arg)
        else: # type(arg) = T
            return self._contains_obj(arg)

    def _add_hash_and_obj(self,hash_val: str,obj: T) -> bool:
        if self.contains(hash_val): # すでに存在
            return False
        else: 
            self._map[hash_val] = obj
            return True

    def _add_obj(self,obj: T) -> bool:
        hash_val = obj.get_hash()
        return self._add_hash_and_obj(hash_val,obj)

    def _add_list(self,objs: list[T]) -> bool:
        success = True
        for obj in objs:
            success &= self._add_obj(obj)

        return success
    
    def add(self,*args) -> bool:
        """ 引数をマップに追加 """
        assert len(args) <= 2        

        if len(args) == 2:
            return self._add_hash_and_obj(args[0],args[1])
        
        if isinstance(args[0],list):
            return self._add_list(args[0])
        else: # type(args[0]) = T
            return self._add_obj(args[0])


    def _remove_hash(self,hash_val: str) -> bool:
        if self.contains(hash_val): # 存在するなら削除
            del self._map[hash_val]
            return True
        else:
            return False

    def _remove_obj(self,obj: T) -> bool:
        hash_val = obj.get_hash()
        return self._remove_hash(hash_val)

    
    def _remove_list(self,objs: list[T]) -> bool:
        success = True
        for obj in objs:
            success &= self._remove_obj(obj)

        return success
    
    def remove(self,arg) -> bool:
        """ 引数をマップから削除 """
        if isinstance(arg,str):
            return self._remove_hash(arg)
        elif isinstance(arg,list):
            return self._remove_list(arg)
        else: # type(arg) = T
            return self._remove_obj(arg)
