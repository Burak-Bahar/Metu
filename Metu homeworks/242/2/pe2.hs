module PE2 where

import Text.Printf

type Point = (Int, Int)
type Dimensions = (Int, Int)
type Vector = (Int, Int)

getRounded :: Double -> Double
getRounded x = read s :: Double
               where s = printf "%.2f" x

castIntToDouble x = (fromIntegral x) :: Double

-------------------------------------------------------------------------------------------
----------------------- DO NOT CHANGE ABOVE OR FUNCTION SIGNATURES-------------------------
------------- DUMMY IMPLEMENTATIONS ARE GIVEN TO PROVIDE A COMPILABLE TEMPLATE ------------
------------------- REPLACE THEM WITH YOUR COMPILABLE IMPLEMENTATIONS ---------------------
-------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------------------------
getVector :: String -> Vector
getVector str
    | (str =="NorthEast" ) = (1,1)
    | str == "NorthWest" = (-1,1)
    | (str == "North") = (0,1)
    | str == "SouthEast" = (1,-1)
    | str ==  "SouthWest" = (-1,-1)
    | str == "South" = (0,-1)
    | (str == "East") = (1,0)
    | str == "West" = (-1,0)
    | otherwise = (0,0)

-------------------------------------------------------------------------------------------------------------------------------
getAllVectors :: [String] -> [Vector]
getAllVectors str_arr = [ getVector str | str <- str_arr]

-------------------------------------------------------------------------------------------------------------------------------
sum1::  Point ->Vector -> Point
sum1 (a,b) (c,d) = (a+c , b+d) 

sum2:: Point -> [Vector] -> [Point]
sum2 (a,b) [] = []
sum2 (a,b) ((c,d):e) = (a+c,b+d): sum2 (a+c,b+d) e

place:: Point -> [Vector] -> [Point]
place initial []= []
place initial (a:b) = (sum2 initial (place initial b))


producePath :: Point -> [String] -> [Point]
producePath initial actions = initial: sum2 initial (getAllVectors actions)

-------------------------------------------------------------------------------------------------------------------------------
inarea:: Dimensions -> [Point] -> [Point]
inarea (a,b) [] = []
inarea (a,b) ((c,d):e) 
    | (c<a && d<b && c>=0 && d>=0) = (c,d): inarea (a,b) e
    | otherwise = []


takePathInArea :: [Point] -> Dimensions -> [Point]
takePathInArea path (bx, by) = inarea (bx,by) path

-------------------------------------------------------------------------------------------------------------------------------
diff:: Point -> [Point] -> Point
diff a [] = a
diff a (b:c) 
    | ( a /= b) = diff a c
    | (a == b) = (0,0)



remainingObjects :: [Point] -> Dimensions -> [Point] -> [Point]
remainingObjects path border objects = [ diff a objects   | a <-(inarea border path) ]

-------------------------------------------------------------------------------------------------------------------------------
averageStepsInSuccess :: [[Point]] -> Dimensions -> [Point] -> Double
averageStepsInSuccess paths border objects = -1.0










