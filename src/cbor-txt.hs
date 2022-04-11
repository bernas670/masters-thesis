import System.Environment
import GhcDump.Ast
import GhcDump.Util
import GhcDump.Repl


-- this is an io action
main = do
    args <- getArgs

    -- make cbor human readable
    mod <- readDump (head args) :: IO Module

    -- save output to file
    writeFile (last args) $ show $ pretty mod
