INF = float("inf")  

# 以下はオブジェクトをスリープ状態にするためのパラメータ
SLEEP_ENERGY_EPSILON = 0.002 # オブジェクトのエネルギーがこの値以下になったらSleep状態にする
MOTION_AVG_BIAS = 0.6        # 0.0から1.0の値を取る移動平均のパラメータ(0.0の場合すぐにSleep状態になる、1.0だとSleepしにくい)

# 以下はSequential Impulse Solverのパラメータ
BETA = 0.01      # めり込みの位置補正項
SLOP = 0.01      # めり込み深さの許容値
SOLVER_ITER = 10 # Sequential Impulse Solverのイテレーション回数

# 減衰パラメータ
LINEAR_DAMPING  = 10  # 速度の減衰パラメータ
ANGULAR_DAMPING = 1e3 # 角速度の減衰パラメータ 
