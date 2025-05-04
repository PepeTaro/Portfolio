import random
import numpy as np

class Qubit:
    def __init__(self,state):
        self.state = None #量子状態
        self.dim   = None #ヒルベルト空間の次元(2のQubit数のべき乗)
        self.num_of_qubits = None #Qubit数        

        self.init(state)

    def __eq__(self,other):
        if isinstance(other,Qubit):
            if self.dim != other.dim or self.num_of_qubits != other.num_of_qubits:
                return False
            
            state = other.state
        else:
            state = other
            
        results = np.isclose(self.state,other)
        return np.all(results == True)

    def __ne__(self,other):
        return not self == other
    
    def __str__(self):
        assert(self.num_of_qubits > 0)
        output = ""
        
        for index in range(self.dim-1):
            output += "({})".format(self.state[index][0]) + self.decimal_to_ket(index) + " + "
        output += "({})".format(self.state[self.dim-1][0]) + self.decimal_to_ket(self.dim-1)
        
        return output
    
    def init(self,state):
        """
        Qubitをstateに付随する情報(ヒルベルト空間の次元、Qubit数)と共に初期化。
        
        Args:
         state: np.ndarray(Qubitの状態をベクトル表示したもの。)
                 
        """
        assert(isinstance(state,np.ndarray))
        self.state = state#量子状態
        self.dim = self.state.shape[0]#ヒルベルト空間の次元(2のQubit数のべき乗)
        self.num_of_qubits = int(np.log2(self.dim))#Qubit数        

    def decimal_to_ket(self,decimal):
        """
        Qubitを10進法表記からKet表記に変換するためのヘルパー。
        戻り値は,文字列であることに注意。
        e.g.) 3 => |011>,2 => |010>.
        Args:
         decimal: int
        
        Returns:
         str
        """
        assert(decimal >= 0 and decimal < self.dim)
        
        format_template = "|{:0" + str(self.num_of_qubits) + "b}>"
        return format_template.format(decimal)

    def observe(self):
        """
        量子状態を観測して、観測されたQubitに対応するインデックスを10進法表記で返す。
        e.g.) 1/2(|00>+|01>+|10>+|11>) の状態において、|01>が観測された場合,1(b01)を返す、|10>なら2(0b10)を返す。
        
        Returns:
         観測されたQubitのインデックス(10進法)。
        
        """
        probs_of_qubits = [] # 各Qubitが観測される各々の確率。
        sum_of_probs = 0.0 # 確率の和、正規化が正しく動作しているならこの値がほぼ１になる。
        for i in range(self.dim):
            prob = np.absolute(self.state[i][0])**2 # 確率は、複素係数の絶対値の２乗であることに注意。
            probs_of_qubits.append(prob)
            sum_of_probs += prob

        assert(np.isclose(sum_of_probs,1.0))# 確率の和がほぼ１か、チェック(そうでないならどこかにバグあり)。
        observed_index = np.random.choice(self.dim,p=probs_of_qubits) # 係数の重みを加えて、サンプリング。
        return observed_index

    def observe_multiple_times(self,num_of_observation):
        """
        現在の量子状態を"壊さず"にnum_of_observation回観測。
        
        Args:
         num_of_observation: int
        
        Returns:
         KeyがQubitのインデックス、Valueが観測回数を格納したdictionaryを返す。
        """
        observation_results = {}

        for i in range(self.dim): # 初期化
            observation_results[i] = 0
        for _ in range(num_of_observation): # 観測
            observed_index = self.observe()
            observation_results[observed_index] += 1 # observed_indexの観測回数をカウント。
            
        return observation_results 

    def get_state_after_observing(self,index):
        """
        indexにより指定されたQubitを観測してその後、残りのQubitを正規化(確率の和が1になるようにする)。
        つまり量子状態を"壊す"。
        [注意] indexは１から始まることに注意。

        Args:
         index: int
        
        Returns:
         観測後の量子状態(ベクトル表記)と観測した古典ビットを返す。
        """
        outcome = self.observe() # 観測
        shift = self.num_of_qubits - index # indexに対応するQubitの観測結果を取り出すために、どのくらいシフトするか計算。
        outcome_bit = (outcome >> shift) & 1 # indexに対応するQubitの観測結果を取得。
        result_state = np.array([],dtype=complex) # 新たな量子状態を格納するための配列を予め確保。
        for i in range(self.dim):
            # indexに対応するQubitの観測結果の値をもつQubitのみをとりだす。
            # つまり,index = 1,outcome_bit = 0 かつ量子状態が、|00>+|01>+|10>+|11>ならば、ループ後の結果は、|00>+|01> としたい。
            if((i >> shift) & 1 == outcome_bit):
                result_state = np.append(result_state,self.state[i])
                
        result_state = result_state/np.sqrt((np.absolute(result_state)**2).sum(0)) # 最後に、量子状態の確率保存のために正規化(確率の和を１にする)。
        result_state = result_state.reshape(-1,1)
        return result_state,outcome_bit
    
    def observe_with_index(self,index):
        """
        indexにより指定されたQubitに対してのみ観測。
        [注意] indexは１から始まることに注意。

        Args:
         index: int
        
        Returns:
         観測した古典ビット。
        """
        assert(self.num_of_qubits > 0 and index >= 1)
        
        result_state,outcome_bit = self.get_state_after_observing(index)
        self.init(result_state)    
        return outcome_bit
    
    def observe_with_indices(self,*indices):
        """
        indicesにより指定された複数のQubitを観測。
        [注意] indicesはすべて１から始まることに注意。

        Args:
         indices: 同時に観測したいQubitのインデックスを格納したtuple。
        
        Returns:
         Keyに観測したQubitのインデックス、ValueにそのQubitを観測した結果(ビット)を格納したdictionary。
        """
            
        sorted_indices = sorted(indices) #昇順にソートされてない場合正しく機能しないため、念のためにソート。
        
        result_bit = {}
        count = 0 # observe_with_index後に量子状態が変わるため、初期のindicesをいくらかシフトするための値を計算。
        for index in sorted_indices:
            shifted_index = index - count 
            outcome = self.observe_with_index(shifted_index)
            result_bit[index] = outcome # indexに対応する観測結果を格納。
            count += 1
        return result_bit
    
   
    def single_qubit_gate(self,gate_matrix,qubit_index):
        """
        https://arxiv.org/pdf/1601.07195.pdf(アルゴリズムの詳細)

        qubit_indexで指定されたQubitに量子ゲートgate_matrixを作用させる。
        [注意]qubit_indexは、1から始まる。
        
        Args:
         gate_matrix: 2x2行列のnp.ndarray
         qubit_index: int
                 
        """
        assert(gate_matrix.shape == (2,2))
        assert(qubit_index >= 1 and qubit_index <= self.num_of_qubits)
        k = self.num_of_qubits  - qubit_index
        result = self.state.copy()
        
        for g in range(0,self.dim,2**(k+1)):
            for i in range(g,g+2**k):
                result[i,0] = gate_matrix[0,0]*self.state[i,0] + gate_matrix[0,1]*self.state[i+2**k,0]
                result[i+2**k,0] = gate_matrix[1,0]*self.state[i,0] + gate_matrix[1,1]*self.state[i+2**k,0]
                
        self.state = result

    
    def two_qubits_gate(self,gate_matrix,control_qubit_index,target_qubit_index):
        """
        https://arxiv.org/pdf/1601.07195.pdf(アルゴリズムの詳細)

        control_qubit_indexはControl Qubit,target_qubit_indexはTarget Qubitにそれぞれ対応。
        control_qubit_indexとtarget_qubit_indexで指定されたQubitを使用してTwo Qubit量子ゲートを作用させる。
        [注意]control_qubit_index,target_qubit_indexは、1から始まる。

        Args:
         gate_matrix: 2x2行列のnp.ndarray
         control_qubit_index: int
         target_qubit_index: int
        """
        assert(gate_matrix.shape == (2,2))
        assert(control_qubit_index >= 1 and control_qubit_index <= self.num_of_qubits)
        assert(target_qubit_index >= 1 and target_qubit_index <= self.num_of_qubits)
        
        c = self.num_of_qubits  - control_qubit_index
        t = self.num_of_qubits  - target_qubit_index
        result = self.state.copy()
        
        for g in range(0,self.dim,2**(t+1)):
            for i in range(g,g+2**t):
                if(i>>c & 0x1 != 0x1):# cの位置のビットが１でないなら、更新しない。
                    continue
                result[i,0] = gate_matrix[0,0]*self.state[i,0] + gate_matrix[0,1]*self.state[i+2**t,0]
                result[i+2**t,0] = gate_matrix[1,0]*self.state[i,0] + gate_matrix[1,1]*self.state[i+2**t,0]
                
        self.state = result

    def oracle(self,function):
        """
        [注意]少なくとも,2つ以上のQubitはエンタングルしている必要がある。
        n個のQubitがエンタングルしている場合、最初のn-1個のQubitを関数functionへの引数とし、
        最後のQubitを,関数へと作用させるQubitとする、
        つまり |x_{1} x_{2} ... x_{n-1}>|y> => |x_{1} x_{2} ... x_{n-1}>|y ^ f(x)>
        関数functionは、function:{0,1}^n -> {0,1}をみたすブール関数。
        
        Args:
         function: ブール関数。
        """
        assert(self.num_of_qubits >= 2)
        new_state = np.zeros_like(self.state) # 新たな量子状態を格納するための配列を予め確保。
        
        for i in range(2**(self.num_of_qubits - 1)):# y = 0の場合
            i <<= 1
            new_state[i + function(i)][0] += self.state[i][0]

        for i in range(2**(self.num_of_qubits - 1)):# y = 1の場合
            i <<= 1
            new_state[i + (1 ^ function(i))][0] += self.state[i+1][0]

        self.init(new_state)
        
    @staticmethod
    def zero():
        """
        Qubit|0>(ベクトル表記では,(1,0)^T,ここで^Tは転置を表す)を返す。
        
        Returns:
         Qubit class
        """
        return Qubit(np.array([[1.0,0.0]],dtype=complex).reshape(2,1))

    @staticmethod
    def one():
        """
        Qubit|1>(ベクトル表記では,(0,1)^T,ここで^Tは転置を表す)を返す。
        
        Returns:
         Qubit class
        """
        return Qubit(np.array([[0.0,1.0]],dtype=complex).reshape(2,1))

    @staticmethod
    def binary(b):
        """
        ２進数文字列からQubitクラスを生成して返す。
        e.g.) Qubit.binary("1011")
        Returns:
         Qubit class
        """
        q = []
        for i in range(len(b)):
            if b[i] == "1":
                q.append(Qubit.one())
            else:
                q.append(Qubit.zero())
            
        x = Qubit.entangle(*q)    
        return x

    @staticmethod
    def entangle(*qbits):
        """
        qbitsにより指定された複数のQubitをエンタングルさせる。

        Args:
         qbits: 複数のQubit classから成るtuple
        
        Returns:
         エンタングルした量子状態が格納されているQubit classを返す。
        """
        entangle = qbits[0]
        for qbit in qbits[1:]:
            entangle = Qubit(np.kron(entangle.state,qbit.state)) # エンタングルするには、クロネッカー積を行う。
        return entangle


