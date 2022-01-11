import math

parameter_a = 1.0
parameter_b = 2.0
parameter_p = 0.0


class Point2D:

    def __init__(self):
        self.x = 0.0
        self.y = 0.0


class Line:

    def __init__(self):
        self.P = Point2D()  # start
        self.Q = Point2D()  # end
        self.M = Point2D()  # mid

        self.len = 0.0
        self.degree = 0.0

    # 已知 PQ 點 算出中點, 長度, 角度
    def PQtoMLD(self):

        self.M.x = (self.P.x + self.Q.x) / 2
        self.M.y = (self.P.y + self.Q.y) / 2

        tmpx = self.Q.x - self.P.x
        tmpy = self.Q.y - self.P.y

        self.len = math.sqrt(tmpx * tmpx + tmpy * tmpy)
        self.degree = math.atan2(tmpy, tmpx)

    # 已知中點,長度,角度 算出PQ點
    def MLDtoPQ(self):

        tmpx = 0.5 * self.len * math.cos(self.degree)
        tmpy = 0.5 * self.len * math.sin(self.degree)

        tmpP = Point2D()
        tmpQ = Point2D()

        tmpP.x = self.M.x - tmpx
        tmpP.y = self.M.y - tmpy
        tmpQ.x = self.M.x + tmpx
        tmpQ.y = self.M.y + tmpy

        self.P = tmpP
        self.Q = tmpQ

    # 傳入一個點 X 算出 X 在此線段的 u
    def Getu(self, X):

        X_P_x = X.x - self.P.x
        X_P_y = X.y - self.P.y
        Q_P_x = self.Q.x - self.P.x
        Q_P_y = self.Q.y - self.P.y

        u = ((X_P_x * Q_P_x) + (X_P_y * Q_P_y)) / (self.len * self.len)

        return u

    # 傳入一個點 X 算出 X 在此線段的 v
    def Getv(self, X):

        X_P_x = X.x - self.P.x
        X_P_y = X.y - self.P.y
        Q_P_x = self.Q.x - self.P.x
        Q_P_y = self.Q.y - self.P.y

        Perp_Q_P_x = Q_P_y
        Perp_Q_P_y = -Q_P_x
        v = ((X_P_x * Perp_Q_P_x) + (X_P_y * Perp_Q_P_y)) / self.len

        return v

    # 傳入u, v 算出此線段上u, v 所產生的新點 X’
    def Get_Point(self, u, v):

        Q_P_x = self.Q.x - self.P.x
        Q_P_y = self.Q.y - self.P.y
        Perp_Q_P_x = Q_P_y
        Perp_Q_P_y = -Q_P_x

        Point_x = self.P.x + u * (self.Q.x - self.P.x) + ((v * Perp_Q_P_x) / self.len)
        Point_y = self.P.y + u * (self.Q.y - self.P.y) + ((v * Perp_Q_P_y) / self.len)

        X = Point2D()
        X.x = Point_x
        X.y = Point_y

        return X

    # 傳入一點 X 算出 X 在此線段所能獲得的 Weight
    def Get_Weight(self, X):

        a = parameter_a
        b = parameter_b
        p = parameter_p
        d = 0.0

        u = self.Getu(X)
        if u > 1.0:
            d = math.sqrt((X.x - self.Q.x) * (X.x - self.Q.x) + (X.y - self.Q.y) * (X.y - self.Q.y))
        elif u < 0:
            d = math.sqrt((X.x - self.P.x) * (X.x - self.P.x) + (X.y - self.P.y) * (X.y - self.P.y))
        else:
            d = abs(self.Getv(X))
        weight = pow(pow(self.len, p) / (a + d), b)

        return weight
