EXEC = P1
CXX = gcc             
OPTS = -O2 -Wall -g   
CFLAGS = $(OPTS)      
LDFLAGS = -lwiringPi -lncurses        


INCDIR = inc
OBJDIR = obj
BINDIR = bin
SRCDIR = src


CFLAGS += -I$(INCDIR) -I$(SRCDIR)


SOURCES = main.c bme280.c bme280_i2c.c crc16.c gpio_pwd.c lcd_i2c.c pid.c uart.c csv_gen.c


_OBJ = $(SOURCES:.c=.o)
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

all: $(BINDIR)/$(EXEC)

$(BINDIR)/$(EXEC): $(OBJ)
		$(CXX) -o $(BINDIR)/$(EXEC) $(OBJ) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
		$(CXX) -c -o $@ $< $(CFLAGS)

clean:
		rm -vf $(BINDIR)/$(EXEC) $(OBJ)