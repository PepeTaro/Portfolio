import logging

logging.basicConfig(format='[%(levelname)-8s] %(message)s',level=logging.DEBUG)
logger = logging.getLogger()
logger.disabled = True # デフォルトでログをオフにする

def log_info(m):
    logging.info(m)

def log_warning(m):
    logging.warning(m)

def log_error(m):
    logging.error(m)

def log_debug(m):
    logging.debug(m)

def turn_off_log():
    logger.disabled = True
    
def turn_on_log():
    logger.disabled = False
