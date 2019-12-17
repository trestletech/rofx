#' Read an OFX/QFX file
#' @export
read_ofx <- function(path){
  li <- ofx_info(normalizePath(path))
  li$transactions <- as.data.frame(li$transactions)
  li
}