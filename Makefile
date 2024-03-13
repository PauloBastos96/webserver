BLUE = \033[0;94m
GREEN = \e[1;92m
RED = \033[0;91m
RESET = \033[1;30m
WHITE = \033[1;97m
YELLOW = \033[1;33m
EMOJI_PACKAGE = \360\237\223\246
EMOJI_HAMMER = \360\237\224\250
EMOJI_TRASH = \360\237\227\221\357\270\217

NAME = webserv

CC = c++
CFLAGS = -Werror -Wextra -Wall -O3 -std=c++98 -g $(INC)

SRCpath_ = src/
OBJpath_ = obj/
LOGSpath_ = logs/
CONFIGSpath_ = configs/

INC = $(addprefix -I, $(shell find . -type d))
SRC = $(wildcard $(SRCpath_)*.cpp $(SRCpath_)**/*.cpp)
OBJ = $(SRC:$(SRCpath_)%.cpp=$(OBJpath_)%.o)

$(OBJpath_)%.o: $(SRCpath_)%.cpp | $(OBJpath_)
	 @mkdir -p $(dir $@)
	 @$(CC) $(CFLAGS) $(INC) -c $< -o $@
	 @printf "$(EMOJI_HAMMER)	$(BLUE)Compiling $(WHITE)$(NAME)		$(BLUE)%-33s$(WHITE)\r" $(notdir $@)

$(LOGSpath_):
	@mkdir -p $(LOGSpath_)
	@touch $(LOGSpath_)webserv.log

$(CONFIGSpath_):
	@mkdir -p $(CONFIGSpath_)
	@touch $(CONFIGSpath_)default.conf

$(OBJpath_):
	@mkdir -p $(OBJpath_)

$(NAME): $(OBJ) $(LOGSpath_) $(CONFIGSpath_)
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJ)
	@printf  "\n$(EMOJI_PACKAGE)	$(WHITE)$(NAME)				$(YELLOW)compiled$(WHITE)\n"

all: $(NAME)

clean:
	@rm -rf $(OBJpath_)
	@rm -rf $(LOGSpath_)

fclean: clean
	@rm -f $(NAME)
	@printf "$(EMOJI_TRASH)	$(WHITE)$(NAME)				$(RED)removed$(WHITE)\n"

re: fclean all

.PHONY: all re clean fclean