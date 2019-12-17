#' Print OFX
#' @export
print_ofx <- function(path){
  li <- ofx_info(normalizePath(path))
  li$transactions <- as.data.frame(li$transactions)
  li
}