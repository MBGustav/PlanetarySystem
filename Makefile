CXX= g++
FLAGS= -Wall -Wextra -std=c++11
INCLUDES= -I./include -I glad/include
LIBS= -L/usr/lib -L/usr/local/lib -lGL -lglfw -lm
SRCDIR= ./src
OBJDIR= ./obj

SOURCES= $(wildcard $(SRCDIR)/*.cpp)

OBJECTS= $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

TARGET= main


all: $(TARGET)


$(TARGET): $(OBJECTS) $(OBJDIR)/glad.o
	$(CXX) $(FLAGS) $(INCLUDES) $(TARGET).cpp $^ -o $@ $(LIBS)



$(OBJECTS): $(SOURCES) $(OBJDIR)
	$(CXX) $(FLAGS) $(INCLUDES) -c $< -o $(OBJDIR)/$(@F)


test: 
	@echo $(SOURCES)
	@echo $(OBJECTS)
	@echo $(OBJDIR)
	@echo $(SRCDIR)/%.cpp
	@echo $(TARGET)



# Compile rule to glad
glad.o : glad/src/glad.c | $(OBJDIR)
	$(CXX) $(FLAGS) -I glad/include -c glad/src/glad.c -o $(OBJDIR)/glad.o


clean:
	rm -rf $(OBJDIR)/* $(TARGET)