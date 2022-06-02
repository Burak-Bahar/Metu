module PE1 where

import Text.Printf
import Data.List

-- Type synonyms --
type Point = (Double, Double)
type Signal = (Double, Double, Double, Double)

-- This function takes a Double and rounds it to 2 decimal places as requested in the PE --
getRounded :: Double -> Double
getRounded x = read s :: Double
               where s = printf "%.2f" x


minu :: Double -> Double -> Double
minu a b = a-b
-------------------------------------------------------------------------------------------
----------------------- DO NOT CHANGE ABOVE OR FUNCTION SIGNATURES-------------------------
------------- DUMMY IMPLEMENTATIONS ARE GIVEN TO PROVIDE A COMPILABLE TEMPLATE ------------
------------------- REPLACE THEM WITH YOUR COMPILABLE IMPLEMENTATIONS ---------------------
-------------------------------------------------------------------------------------------

getDistance :: Point -> Point -> Double
getDistance (x1, y1) (x2, y2) = getRounded (sqrt $ (x1-x2)**2 + (y1-y2)**2)

-------------------------------------------------------------------------------------------

findAllDistances :: Point -> [Point] -> [Double]
findAllDistances b l = [getDistance b a| a <- l]

-------------------------------------------------------------------------------------------
mm:: [Double] -> Double
mm [l] = l
mm (l:ls)
    | (mm ls) > l = mm ls
    | otherwise = l
    
    
mim:: [Double] -> Double
mim [l] = l
mim (l:ls)
    | (mim ls) < l = mim ls
    | otherwise = l
    
    
maxi:: Point -> [Point] -> Double -> [Point]
maxi b l mm = [a | a <- l, (getDistance b a) == mm]

mini:: Point -> [Point] -> Double -> [Point]
mini b l mm = [a | a <- l, (getDistance b a) == mm]

findExtremes :: Point -> [Point] -> (Point, Point)
findExtremes b l = ( mini b l (mim(findAllDistances b l))!!0, maxi b l (mm(findAllDistances b l))!!0)

--------------------------------------------------------------------------------------------


getSingleAction :: Signal -> String
getSingleAction (a,b,c,d) 
    | (a-c > 0 && b-d > 0) = "NorthEast"
    | a-c > 0 && b-d < 0 = "NorthWest"
    | (a-c > 0 && b-d == 0) = "North"
    | a-c < 0 && b-d > 0 = "SouthEast"
    | a-c < 0 && b-d < 0 = "SouthWest"
    | a-c < 0 && b-d == 0 = "South"
    | (a-c == 0 && b-d > 0) = "East"
    | a-c == 0 && b-d < 0 = "West"
    | otherwise = "Stay"
    
-------------------------------------------------------------------------------------------

getAllActions :: [Signal] -> [String]
getAllActions signals = [getSingleAction sign | sign <- signals ]

-------------------------------------------------------------------------------------------
inc:: [Int] -> Int
inc [] = 0
inc (a:b) = a+ inc b

gg :: Int -> [String] -> String -> [Int]
gg a dest action = [1 | b <- dest, b==action]

numberOfGivenAction :: Num a => [Signal] -> String -> a
numberOfGivenAction signals action = fromIntegral (inc(gg 0 (getAllActions signals) action))






