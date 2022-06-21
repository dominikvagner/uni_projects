# Interpret for IPPcode22 language parsed as XML.
#     ___ _   __
#    / _ \ | / /
#   / // / |/ /
#  /____/|___/
#
# @author Dominik Vagner, xvagne10

#****************************** IMPORTS ******************************#
import enum
import getopt
import re
import sys
import xml.etree.ElementTree as Et
from typing import List, Dict, Any, TextIO, Union


#******************************* ENUMS *******************************#
class ExitCodes(enum.Enum):
    MISSING_PARAM = 10
    INPUT_FILE_ERR = 11
    OUTPUT_FILE_ERR = 12
    BAD_FORMED_XML = 31
    WRONG_XML_STRUCT = 32
    SEMANTIC_ERR = 52           # Undefined label, <var> redefinition etc.
    WRONG_OPERAND_TYPE = 53
    UNDEFINED_VAR = 54
    FRAME_DOESNT_EXIST = 55
    MISSING_VALUE = 56          # In <var>, data_stack or call_stack.
    WRONG_OPERAND_VALUE = 57    # Zero division, wrong EXIT value.
    RUNTIME_INTERPRET_ERR = 58  # Wrong string usage.


class DataTypes(enum.Enum):
    INT = 'int'
    STRING = 'string'
    BOOL = 'bool'
    NIL = 'nil'


class ArgTypes(enum.Enum):
    VARIABLE = 'var'
    SYMBOL = 'symb'
    LABEL = 'label'
    TYPE = 'type'


class FrameTypes(enum.Enum):
    GLOBAL = 'GF'
    LOCAL = 'LF'
    TEMP = 'TF'


#************************** HELPER FUNCTIONS **************************#
def print_help():
    print("""Usage: interpret.py --source <sourcefile> --input <inputfile>\n
arguments:
--help      shows this help and exits, can't be combined with other arguments
--source=   requires value, value specifies from which file the XML is read
--input=    requires value, value specifies from which file is the user input for the program read
! at least one of the source or input arguments need to be specified other is then read from stdin""")
    exit(0)


def my_exit(code, msg=''):
    sys.stderr.write(msg + '\n')
    sys.exit(code.value)


#***************************** ARGUMENTS *****************************#
class Argument:
    value: Any


class ArgVariable(Argument):
    def __init__(self, frame: FrameTypes, value: str):
        self.frame = frame
        self.value = value


class ArgSymbol(Argument):
    def __init__(self, typ: Union[DataTypes, None], value: Any):
        self.typ = typ
        self.value = value


class ArgLabel(Argument):
    def __init__(self, value: str):
        self.value = value


class ArgType(Argument):
    def __init__(self, value: DataTypes):
        self.value = value


#**************************** INSTRUCTIONS ****************************#
class Instruction:
    opcode: str = None
    args: List[Argument] = None

    def __init__(self, opcode: str, args: List[Argument]):
        self.opcode = opcode
        self.args = args

    def execute(self, p):
        pass

    def get_opcode(self) -> str:
        return self.opcode

    def get_args(self) -> List[Argument]:
        return self.args


#************************** PROGRAM EXECUTE **************************#
class ProgramExecutor:
    instruction_dict: Dict[int, Instruction]
    label_dict: Dict[str, int]
    global_frame: Dict[str, ArgSymbol]
    temp_frame: Union[Dict[str, ArgSymbol], None]
    local_frame: List[Dict[str, ArgSymbol]]
    current_inst: int
    final_inst: int
    d_stack: List
    c_stack: List
    input: TextIO
    ki = Dict
    ik = Dict

    def __init__(self, instructions: Dict[int, Instruction], read_input):
        self.instruction_dict = instructions
        self.global_frame = {}
        self.temp_frame = None
        self.local_frame = []
        self.current_inst = list(self.instruction_dict.keys())[0]
        self.final_inst = list(self.instruction_dict.keys())[-1]
        self.d_stack = []
        self.c_stack = []
        self.get_labels()
        self.input = read_input
        self.ki = dict()
        self.ik = dict()

    def get_labels(self):
        self.label_dict = {}
        for order, inst in self.instruction_dict.items():
            if type(inst) == Label:
                name = getattr(inst.args[0], 'value')
                if name in self.label_dict.keys():
                    my_exit(ExitCodes.SEMANTIC_ERR, "Semantic error, label redefinition at two different places.")
                self.label_dict[name] = order

    def exec(self):
        for i, k in enumerate(self.instruction_dict):
            self.ki[k] = i
            self.ik[i] = k
        while self.current_inst:
            instruction = self.instruction_dict[self.current_inst]
            instruction.execute(p=self)
            index_of_current_inst = self.ki[self.current_inst]
            index_of_next_inst = index_of_current_inst + 1
            self.current_inst = self.ik[index_of_next_inst] if index_of_next_inst in self.ik else None

    def is_var_defined(self, var: Argument):
        frame = getattr(var, 'frame')
        value = getattr(var, 'value')

        if frame == FrameTypes.GLOBAL:
            return self.global_frame.__contains__(value)
        elif frame == FrameTypes.LOCAL:
            if len(self.local_frame) == 0:
                my_exit(ExitCodes.FRAME_DOESNT_EXIST, "Runtime error, frame doesn't exist.")
            return self.local_frame[-1].__contains__(value)
        else:
            if self.temp_frame is None:
                my_exit(ExitCodes.FRAME_DOESNT_EXIST, "Runtime error, frame doesn't exist.")
            return self.temp_frame.__contains__(value)

    def fill_var(self, var: Argument, symb: ArgSymbol):
        frame = getattr(var, 'frame')
        value = getattr(var, 'value')

        if frame == FrameTypes.GLOBAL:
            self.global_frame[value] = symb
        elif frame == FrameTypes.LOCAL:
            if len(self.local_frame) == 0:
                my_exit(ExitCodes.FRAME_DOESNT_EXIST, "Runtime error, frame doesn't exist.")
            self.local_frame[-1][value] = symb
        else:
            if self.temp_frame is None:
                my_exit(ExitCodes.FRAME_DOESNT_EXIST, "Runtime error, frame doesn't exist.")
            self.temp_frame[value] = symb

    def get_symb_from_var(self, var: Argument) -> ArgSymbol:
        frame = getattr(var, 'frame')
        value = getattr(var, 'value')

        if frame == FrameTypes.GLOBAL:
            return self.global_frame[value]
        elif frame == FrameTypes.LOCAL:
            if len(self.local_frame) == 0:
                my_exit(ExitCodes.FRAME_DOESNT_EXIST, "Runtime error, frame doesn't exist.")
            return self.local_frame[-1][value]
        else:
            if self.temp_frame is None:
                my_exit(ExitCodes.FRAME_DOESNT_EXIST, "Runtime error, frame doesn't exist.")
            return self.temp_frame[value]


class Move(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb = self.args[1]
        if type(symb) == ArgVariable:
            if not p.is_var_defined(symb):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb = p.get_symb_from_var(symb)
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        if symb.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in move the moved operand is unset.")
        p.fill_var(var, symb)


class Createframe(Instruction):
    def execute(self, p: ProgramExecutor):
        p.temp_frame = dict()


class Pushframe(Instruction):
    def execute(self, p: ProgramExecutor):
        if p.temp_frame is None:
            my_exit(ExitCodes.FRAME_DOESNT_EXIST, "Runtime error, frame doesn't exist.")
        p.local_frame.append(p.temp_frame)
        p.temp_frame = None


class Popframe(Instruction):
    def execute(self, p: ProgramExecutor):
        if len(p.local_frame) == 0:
            my_exit(ExitCodes.FRAME_DOESNT_EXIST, "Runtime error, frame doesn't exist.")
        p.temp_frame = p.local_frame.pop()


class Defvar(Instruction):
    def execute(self, p: ProgramExecutor):
        if p.is_var_defined(self.args[0]):
            my_exit(ExitCodes.SEMANTIC_ERR, 'Semantic error, variable redefinition')
        p.fill_var(self.args[0], ArgSymbol(None, None))


class Call(Instruction):
    def execute(self, p: ProgramExecutor):
        index_of_current_inst = p.ki[p.current_inst]
        index_of_next_inst = index_of_current_inst
        called_from = p.ik[index_of_next_inst] if index_of_next_inst in p.ik else None
        p.c_stack.append(called_from)
        value = getattr(self.args[0], 'value')
        if not p.label_dict.__contains__(value):
            my_exit(ExitCodes.SEMANTIC_ERR, "Semantic error, undefined label.")
        p.current_inst = p.label_dict[value]


class Return(Instruction):
    def execute(self, p: ProgramExecutor):
        return_to = None
        try:
            return_to = p.c_stack.pop()
        except IndexError:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, missing value for return.")
        p.current_inst = return_to


class PushS(Instruction):
    def execute(self, p: ProgramExecutor):
        symb = self.args[0]
        if type(symb) == ArgVariable:
            if not p.is_var_defined(symb):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb = p.get_symb_from_var(symb)
        if symb.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, unset variable.")
        p.d_stack.append(symb)


class PopS(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        if len(p.d_stack) == 0:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, missing symbol in data stack.")
        p.fill_var(var, p.d_stack.pop())


class Add(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb1 = self.args[1]
        symb2 = self.args[2]
        if type(symb1) == ArgVariable:
            if not p.is_var_defined(symb1):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb1 = p.get_symb_from_var(symb1)
        if type(symb2) == ArgVariable:
            if not p.is_var_defined(symb2):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb2 = p.get_symb_from_var(symb2)
        if symb1.typ is None or symb2.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in arithmetic at least one of the operands is unset.")
        if symb1.typ != DataTypes.INT or symb2.typ != DataTypes.INT:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in arithmetic at least one of the operands isn't int.")
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        p.fill_var(var, ArgSymbol(DataTypes.INT, (symb1.value + symb2.value)))


class Sub(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb1 = self.args[1]
        symb2 = self.args[2]
        if type(symb1) == ArgVariable:
            if not p.is_var_defined(symb1):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb1 = p.get_symb_from_var(symb1)
        if type(symb2) == ArgVariable:
            if not p.is_var_defined(symb2):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb2 = p.get_symb_from_var(symb2)
        if symb1.typ is None or symb2.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in arithmetic at least one of the operands is unset.")
        if symb1.typ != DataTypes.INT or symb2.typ != DataTypes.INT:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in arithmetic at least one of the operands isn't int.")
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        p.fill_var(var, ArgSymbol(DataTypes.INT, (symb1.value - symb2.value)))


class Mul(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb1 = self.args[1]
        symb2 = self.args[2]
        if type(symb1) == ArgVariable:
            if not p.is_var_defined(symb1):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb1 = p.get_symb_from_var(symb1)
        if type(symb2) == ArgVariable:
            if not p.is_var_defined(symb2):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb2 = p.get_symb_from_var(symb2)
        if symb1.typ is None or symb2.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in arithmetic at least one of the operands is unset.")
        if symb1.typ != DataTypes.INT or symb2.typ != DataTypes.INT:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in arithmetic at least one of the operands isn't int.")
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        p.fill_var(var, ArgSymbol(DataTypes.INT, (symb1.value * symb2.value)))


class IDiv(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb1 = self.args[1]
        symb2 = self.args[2]
        if type(symb1) == ArgVariable:
            if not p.is_var_defined(symb1):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb1 = p.get_symb_from_var(symb1)
        if type(symb2) == ArgVariable:
            if not p.is_var_defined(symb2):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb2 = p.get_symb_from_var(symb2)
        if symb1.typ is None or symb2.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in arithmetic at least one of the operands is unset.")
        if symb1.typ != DataTypes.INT or symb2.typ != DataTypes.INT:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in arithmetic at least one of the operands isn't int.")
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        if symb2.value == 0:
            my_exit(ExitCodes.WRONG_OPERAND_VALUE, "Runtime error, zero division error.")
        p.fill_var(var, ArgSymbol(DataTypes.INT, (symb1.value // symb2.value)))


class Lt(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb1 = self.args[1]
        symb2 = self.args[2]
        if type(symb1) == ArgVariable:
            if not p.is_var_defined(symb1):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb1 = p.get_symb_from_var(symb1)
        if type(symb2) == ArgVariable:
            if not p.is_var_defined(symb2):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb2 = p.get_symb_from_var(symb2)
        if symb1.typ is None or symb2.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in comparison at least one of the operands is unset.")
        if not symb1.typ == symb2.typ:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in comparison the operands aren't of the same type.")
        if symb1.typ == DataTypes.NIL or symb2.typ == DataTypes.NIL:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in LT at least one operand is nil.")
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        p.fill_var(var, ArgSymbol(DataTypes.BOOL, symb1.value < symb2.value))


class Gt(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb1 = self.args[1]
        symb2 = self.args[2]
        if type(symb1) == ArgVariable:
            if not p.is_var_defined(symb1):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb1 = p.get_symb_from_var(symb1)
        if type(symb2) == ArgVariable:
            if not p.is_var_defined(symb2):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb2 = p.get_symb_from_var(symb2)
        if symb1.typ is None or symb2.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in comparison at least one of the operands is unset.")
        if not symb1.typ == symb2.typ:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in comparison the operands aren't of the same type.")
        if symb1.typ == DataTypes.NIL or symb2.typ == DataTypes.NIL:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in LT at least one operand is nil.")
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        p.fill_var(var, ArgSymbol(DataTypes.BOOL, symb1.value > symb2.value))


class Eq(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb1 = self.args[1]
        symb2 = self.args[2]
        if type(symb1) == ArgVariable:
            if not p.is_var_defined(symb1):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb1 = p.get_symb_from_var(symb1)
        if type(symb2) == ArgVariable:
            if not p.is_var_defined(symb2):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb2 = p.get_symb_from_var(symb2)
        if symb1.typ is None or symb2.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in comparison at least one of the operands is unset.")
        if not symb1.typ == symb2.typ:
            if symb1.typ != DataTypes.NIL and symb2.typ != DataTypes.NIL:
                my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in comparison the operands aren't of the same type.")
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        p.fill_var(var, ArgSymbol(DataTypes.BOOL, symb1.value == symb2.value))


class And(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb1 = self.args[1]
        symb2 = self.args[2]
        if type(symb1) == ArgVariable:
            if not p.is_var_defined(symb1):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb1 = p.get_symb_from_var(symb1)
        if type(symb2) == ArgVariable:
            if not p.is_var_defined(symb2):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb2 = p.get_symb_from_var(symb2)
        if symb1.typ is None or symb2.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in comparison at least one of the operands is unset.")
        if symb1.typ != DataTypes.BOOL or symb2.typ != DataTypes.BOOL:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in comparison the operands aren't booleans.")
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        p.fill_var(var, ArgSymbol(DataTypes.BOOL, symb1.value and symb2.value))


class Or(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb1 = self.args[1]
        symb2 = self.args[2]
        if type(symb1) == ArgVariable:
            if not p.is_var_defined(symb1):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb1 = p.get_symb_from_var(symb1)
        if type(symb2) == ArgVariable:
            if not p.is_var_defined(symb2):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb2 = p.get_symb_from_var(symb2)
        if symb1.typ is None or symb2.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in comparison at least one of the operands is unset.")
        if symb1.typ != DataTypes.BOOL or symb2.typ != DataTypes.BOOL:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in comparison the operands aren't booleans.")
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        p.fill_var(var, ArgSymbol(DataTypes.BOOL, symb1.value or symb2.value))


class Not(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb1 = self.args[1]
        if type(symb1) == ArgVariable:
            if not p.is_var_defined(symb1):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb1 = p.get_symb_from_var(symb1)
        if symb1.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in negation at least one of the operands is unset.")
        if symb1.typ != DataTypes.BOOL:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in negation the operand isn' boolean.")
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        p.fill_var(var, ArgSymbol(DataTypes.BOOL, not symb1.value))


class Int2Char(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb = self.args[1]
        if type(symb) == ArgVariable:
            if not p.is_var_defined(symb):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb = p.get_symb_from_var(symb)
        if symb.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, unset value.")
        if symb.typ != DataTypes.INT:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in int2char the operand isn't int.")
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        try:
            result = chr(symb.value)
            p.fill_var(var, ArgSymbol(DataTypes.STRING, result))
        except ValueError:
            my_exit(ExitCodes.RUNTIME_INTERPRET_ERR, "Runtime error, in INT2CHAR int out of range.")


class Stri2Int(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb1 = self.args[1]
        symb2 = self.args[2]
        if type(symb1) == ArgVariable:
            if not p.is_var_defined(symb1):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb1 = p.get_symb_from_var(symb1)
        if type(symb2) == ArgVariable:
            if not p.is_var_defined(symb2):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb2 = p.get_symb_from_var(symb2)
        if symb1.typ is None or symb2.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in arithmetic at least one of the operands is unset.")
        if symb1.typ != DataTypes.STRING or symb2.typ != DataTypes.INT:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE,
                    "Runtime error, in str2int at least one of the operands isn't of correct type.")
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        try:
            l_symb = list(symb1.value)
            if symb2.value < 0:
                raise IndexError
            result = ord(l_symb[symb2.value])
            p.fill_var(var, ArgSymbol(DataTypes.INT, result))
        except ValueError:
            my_exit(ExitCodes.RUNTIME_INTERPRET_ERR, "Runtime error, in stri2int char out of range.")
        except IndexError:
            my_exit(ExitCodes.RUNTIME_INTERPRET_ERR, "Runtime error, in stri2int int out of range.")


class Read(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        typ = self.args[1]
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        red = p.input.readline()
        if len(red) == 0:
            p.fill_var(var, ArgSymbol(DataTypes.NIL, None))
        elif typ.value == DataTypes.INT:
            red = red.rstrip()
            try:
                red = int(red)
                p.fill_var(var, ArgSymbol(DataTypes.INT, red))
            except ValueError:
                p.fill_var(var, ArgSymbol(DataTypes.NIL, None))
        elif typ.value == DataTypes.BOOL:
            red = red.rstrip()
            if red.lower() == 'true':
                p.fill_var(var, ArgSymbol(DataTypes.BOOL, True))
            else:
                p.fill_var(var, ArgSymbol(DataTypes.BOOL, False))
        elif typ.value == DataTypes.STRING:
            if len(red) == 1:
                p.fill_var(var, ArgSymbol(DataTypes.STRING, ''))
            else:
                red = red.rstrip()
                p.fill_var(var, ArgSymbol(DataTypes.STRING, re.sub(r'\\([0-9]{3})', lambda x: chr(int(x[1])), red)))


class Write(Instruction):
    def execute(self, p: ProgramExecutor):
        symb = self.args[0]
        if type(symb) == ArgVariable:
            if not p.is_var_defined(symb):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb = p.get_symb_from_var(symb)
        symb_type = symb.typ
        if symb_type is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, unset variable.")
        if symb_type == DataTypes.NIL:
            print('', end='')
        elif symb_type == DataTypes.BOOL:
            if symb.value:
                print('true', end='')
            else:
                print('false', end='')
        else:
            print(symb.value, end='')


class Concat(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb1 = self.args[1]
        symb2 = self.args[2]
        if type(symb1) == ArgVariable:
            if not p.is_var_defined(symb1):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb1 = p.get_symb_from_var(symb1)
        if type(symb2) == ArgVariable:
            if not p.is_var_defined(symb2):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb2 = p.get_symb_from_var(symb2)
        if symb1.typ is None or symb2.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in concat at least one of the operands is unset.")
        if symb1.typ != DataTypes.STRING or symb2.typ != DataTypes.STRING:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in concat at least on of the operands isn't string.")
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        p.fill_var(var, ArgSymbol(DataTypes.STRING, symb1.value + symb2.value))


class Strlen(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb = self.args[1]
        if type(symb) == ArgVariable:
            if not p.is_var_defined(symb):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb = p.get_symb_from_var(symb)
        if symb.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in strlen string symb is unset.")
        if symb.typ != DataTypes.STRING:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in concat at least on of the operands isn't string.")
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        p.fill_var(var, ArgSymbol(DataTypes.INT, len(symb.value)))


class Getchar(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb1 = self.args[1]
        symb2 = self.args[2]
        if type(symb1) == ArgVariable:
            if not p.is_var_defined(symb1):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb1 = p.get_symb_from_var(symb1)
        if type(symb2) == ArgVariable:
            if not p.is_var_defined(symb2):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb2 = p.get_symb_from_var(symb2)
        if symb1.typ is None or symb2.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in arithmetic at least one of the operands is unset.")
        if symb1.typ != DataTypes.STRING or symb2.typ != DataTypes.INT:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE,
                    "Runtime error, in str2int at least one of the operands isn't of correct type.")
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        try:
            l_symb = list(symb1.value)
            if symb2.value < 0:
                raise IndexError
            result = l_symb[symb2.value]
            p.fill_var(var, ArgSymbol(DataTypes.STRING, result))
        except ValueError:
            my_exit(ExitCodes.RUNTIME_INTERPRET_ERR, "Runtime error, in stri2int char out of range.")
        except IndexError:
            my_exit(ExitCodes.RUNTIME_INTERPRET_ERR, "Runtime error, in stri2int int out of range.")


class Setchar(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb1 = self.args[1]
        symb2 = self.args[2]
        if type(symb1) == ArgVariable:
            if not p.is_var_defined(symb1):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb1 = p.get_symb_from_var(symb1)
        if type(symb2) == ArgVariable:
            if not p.is_var_defined(symb2):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb2 = p.get_symb_from_var(symb2)
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        var_symb = p.get_symb_from_var(var)
        if symb1.typ is None or symb2.typ is None or var_symb.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in arithmetic at least one of the operands is unset.")
        if var_symb.typ != DataTypes.STRING or symb1.typ != DataTypes.INT or symb2.typ != DataTypes.STRING:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE,
                    "Runtime error, in str2int at least one of the operands isn't of correct type.")
        try:
            l_symb = list(var_symb.value)
            if symb1.value < 0:
                raise IndexError
            l_symb[symb1.value] = symb2.value[0]
            p.fill_var(var, ArgSymbol(DataTypes.STRING, ''.join([str(elem) for elem in l_symb])))
        except ValueError:
            my_exit(ExitCodes.RUNTIME_INTERPRET_ERR, "Runtime error, in stri2int char out of range.")
        except IndexError:
            my_exit(ExitCodes.RUNTIME_INTERPRET_ERR, "Runtime error, in stri2int int out of range.")


class Type(Instruction):
    def execute(self, p: ProgramExecutor):
        var = self.args[0]
        symb1 = self.args[1]
        if not p.is_var_defined(var):
            my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
        if type(symb1) == ArgVariable:
            if not p.is_var_defined(symb1):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb1 = p.get_symb_from_var(symb1)
        if symb1.typ is None:
            p.fill_var(var, ArgSymbol(DataTypes.STRING, ''))
        elif symb1.typ == DataTypes.INT:
            p.fill_var(var, ArgSymbol(DataTypes.STRING, 'int'))
        elif symb1.typ == DataTypes.NIL:
            p.fill_var(var, ArgSymbol(DataTypes.STRING, 'nil'))
        elif symb1.typ == DataTypes.STRING:
            p.fill_var(var, ArgSymbol(DataTypes.STRING, 'string'))
        elif symb1.typ == DataTypes.BOOL:
            p.fill_var(var, ArgSymbol(DataTypes.STRING, 'bool'))


class Label(Instruction):
    def execute(self, p: ProgramExecutor):
        pass


class Jump(Instruction):
    def execute(self, p: ProgramExecutor):
        value = getattr(self.args[0], 'value')
        if not p.label_dict.__contains__(value):
            my_exit(ExitCodes.SEMANTIC_ERR, "Semantic error, undefined label.")
        p.current_inst = p.label_dict[value]


class JumpIfEq(Instruction):
    def execute(self, p: ProgramExecutor):
        symb1 = self.args[1]
        symb2 = self.args[2]
        if type(symb1) == ArgVariable:
            if not p.is_var_defined(symb1):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb1 = p.get_symb_from_var(symb1)
        if type(symb2) == ArgVariable:
            if not p.is_var_defined(symb2):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb2 = p.get_symb_from_var(symb2)
        if not p.label_dict.__contains__(getattr(self.args[0], 'value')) or len(p.label_dict) == 0:
            my_exit(ExitCodes.SEMANTIC_ERR, "Semantic error, undefined label.")
        if symb1.typ is None or symb2.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in comparison at least one of the operands is unset.")
        if symb1.typ == symb2.typ or symb1.typ == DataTypes.NIL or symb2.typ == DataTypes.NIL:
            if symb1.value == symb2.value:
                p.current_inst = p.label_dict[getattr(self.args[0], 'value')]
        else:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in comparison one symbol aren't of the same type.")


class JumpIfNeq(Instruction):
    def execute(self, p: ProgramExecutor):
        symb1 = self.args[1]
        symb2 = self.args[2]
        if type(symb1) == ArgVariable:
            if not p.is_var_defined(symb1):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb1 = p.get_symb_from_var(symb1)
        if type(symb2) == ArgVariable:
            if not p.is_var_defined(symb2):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb2 = p.get_symb_from_var(symb2)
        if not p.label_dict.__contains__(getattr(self.args[0], 'value')) or len(p.label_dict) == 0:
            my_exit(ExitCodes.SEMANTIC_ERR, "Semantic error, undefined label.")
        if symb1.typ is None or symb2.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, in comparison at least one of the operands is unset.")
        if symb1.typ == symb2.typ or symb1.typ == DataTypes.NIL or symb2.typ == DataTypes.NIL:
            if symb1.value != symb2.value:
                p.current_inst = p.label_dict[getattr(self.args[0], 'value')]
        else:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in comparison one symbol aren't of the same type.")


class Exit(Instruction):
    def execute(self, p: ProgramExecutor):
        symb = self.args[0]
        if type(symb) == ArgVariable:
            if not p.is_var_defined(symb):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb = p.get_symb_from_var(symb)
        if symb.typ is None:
            my_exit(ExitCodes.MISSING_VALUE, "Runtime error, unset variable.")
        if symb.typ != DataTypes.INT:
            my_exit(ExitCodes.WRONG_OPERAND_TYPE, "Runtime error, in exit the operand isn't int.")
        if symb.value < 0 or symb.value > 49:
            my_exit(ExitCodes.WRONG_OPERAND_VALUE, "Runtime error, in exit the operand isn't in <0, 49>.")
        my_exit(symb, f"Runtime error, exited on EXIT instruction with the code: {symb.value}")


class DPrint(Instruction):
    def execute(self, p: ProgramExecutor):
        symb = self.args[0]
        if type(symb) == ArgVariable:
            if not p.is_var_defined(symb):
                my_exit(ExitCodes.UNDEFINED_VAR, "Runtime error, undefined variable.")
            symb = p.get_symb_from_var(symb)
        sys.stderr.write(f"Symb type: {symb.typ.value}\nSymb value: {symb.value}\n")


class Break(Instruction):
    def execute(self, p: ProgramExecutor):
        sys.stderr.write(
            f"Current instruction order: {p.current_inst}\n"
            f"Current global frame state: {p.global_frame}\n"
            f"Current local frame state: {p.local_frame}\n"
            f"Current temp frame state: {p.temp_frame}\n"
        )


#**************************** XML PARSING ****************************#
class XmlParser:
    inst_dict: Dict[int, Instruction] = {}
    sr: Et.Element = None

    def __init__(self, source_file):
        try:
            self.sr = Et.parse(source_file).getroot()
        except Et.ParseError:
            my_exit(ExitCodes.BAD_FORMED_XML, "XML file isn't well formed.")

    def parse(self):
        if self.sr.tag != 'program':
            my_exit(ExitCodes.WRONG_XML_STRUCT, "Root XML element isn't named program.")

        if not self.sr.attrib.__contains__('language'):
            my_exit(ExitCodes.WRONG_XML_STRUCT, "Invalid XML structure, program doesn't have language attribute.")
        if self.sr.attrib['language'] != 'IPPcode22':
            my_exit(ExitCodes.WRONG_XML_STRUCT, "Invalid XML structure, language attribute isn't IPPcode22")

        for child in self.sr:
            if child.tag != 'instruction':
                my_exit(ExitCodes.WRONG_XML_STRUCT, "Invalid XML structure, child other than 'instruction'.")

            if not child.attrib.__contains__('opcode'):
                my_exit(ExitCodes.WRONG_XML_STRUCT, "Invalid XML structure, instruction doesn't have opcode.")
            if not opcode_info.__contains__(child.attrib['opcode'].upper()):
                my_exit(ExitCodes.WRONG_XML_STRUCT, "Invalid XML structure, unknown opcode.")

            if not child.attrib.__contains__('order'):
                my_exit(ExitCodes.WRONG_XML_STRUCT, "Invalid XML structure, instruction doesn't have order.")

            order: int = 0
            try:
                order = int(child.attrib['order'])
            except ValueError:
                my_exit(ExitCodes.WRONG_XML_STRUCT, "Invalid XML structure, order isn't int.")
            if order <= 0:
                my_exit(ExitCodes.WRONG_XML_STRUCT, "Invalid XML structure, negative or zero order.")
            if self.inst_dict.__contains__(order):
                my_exit(ExitCodes.WRONG_XML_STRUCT, "Invalid XML structure, duplicate order.")

            opcode = child.attrib['opcode'].upper()
            expected = opcode_info[opcode]
            expected_class = expected[0]
            expected_count = expected[1]
            expected_types = expected[2]

            args = self.parse_args(child, expected_count, list(expected_types))

            self.inst_dict[order] = expected_class(opcode, args)

        if len(self.inst_dict) == 0:
            my_exit(ArgSymbol(None, 0), "No instructions.")
        # noinspection PyTypeChecker
        self.inst_dict = dict(sorted(self.inst_dict.items()))

    def parse_args(self, element: Et.Element, count: int, types: List[Argument]) -> List[Argument]:
        args = []
        tags = ([elem.tag for elem in element.iter()][1:])
        if tags and not all(tag in ['arg1', 'arg2', 'arg3'] for tag in tags):
            my_exit(ExitCodes.WRONG_XML_STRUCT, 'Invalid XML structure, different argument tag than expected.')
        tags.sort()

        if count == 0:
            if len(tags) != 0:
                my_exit(ExitCodes.WRONG_XML_STRUCT, 'Invalid XML structure, wrong amount of arguments.')

        elif count == 1:
            if len(tags) != 1:
                my_exit(ExitCodes.WRONG_XML_STRUCT, 'Invalid XML structure, wrong amount of arguments.')
            if tags[0] != 'arg1':
                my_exit(ExitCodes.WRONG_XML_STRUCT, 'Invalid XML structure, wrong argument numbers.')
            args.append(self.parse_arg(element.find('arg1'), types[0]))

        elif count == 2:
            if len(tags) != 2:
                my_exit(ExitCodes.WRONG_XML_STRUCT, 'Invalid XML structure, wrong amount of arguments.')
            if tags[0] != 'arg1' or tags[1] != 'arg2':
                my_exit(ExitCodes.WRONG_XML_STRUCT, 'Invalid XML structure, wrong argument numbers.')
            args.append(self.parse_arg(element.find('arg1'), types[0]))
            args.append(self.parse_arg(element.find('arg2'), types[1]))

        elif count == 3:
            if len(tags) != 3:
                my_exit(ExitCodes.WRONG_XML_STRUCT, 'Invalid XML structure, wrong amount of arguments.')
            if tags[0] != 'arg1' or tags[1] != 'arg2' or tags[2] != 'arg3':
                my_exit(ExitCodes.WRONG_XML_STRUCT, 'Invalid XML structure, wrong argument numbers.')
            args.append(self.parse_arg(element.find('arg1'), types[0]))
            args.append(self.parse_arg(element.find('arg2'), types[1]))
            args.append(self.parse_arg(element.find('arg3'), types[2]))

        return args

    @staticmethod
    def parse_arg(element: Et.Element, typ: Argument) -> Argument:
        arg: Union[Argument, None] = None

        if not element.attrib.__contains__('type'):
            my_exit(ExitCodes.WRONG_XML_STRUCT, "Invalid XML structure, argument tag doesn't have type attribute.")

        type_attrib = element.attrib['type']
        if typ == ArgVariable:
            split = element.text.split('@', 1)
            arg = ArgVariable(FrameTypes(split[0]), split[1])
        elif typ == ArgSymbol:
            if type_attrib == 'var':
                split = element.text.split('@', 1)
                arg = ArgVariable(FrameTypes(split[0]), split[1])
                return arg
            elif type_attrib == 'int':
                try:
                    arg = ArgSymbol(DataTypes.INT, int(element.text))
                except ValueError:
                    my_exit(ExitCodes.WRONG_XML_STRUCT, "Invalid XML structure, invalid value for int type.")
            elif type_attrib == 'string':
                if element.text is None:
                    arg = ArgSymbol(DataTypes.STRING, '')
                elif type(element.text) == str:
                    arg = ArgSymbol(DataTypes.STRING, re.sub(r'\\([0-9]{3})', lambda x: chr(int(x[1])), element.text))
                else:
                    arg = ArgSymbol(DataTypes.STRING, element.text)
            elif type_attrib == 'bool':
                if element.text.lower() == 'true':
                    arg = ArgSymbol(DataTypes.BOOL, True)
                else:
                    arg = ArgSymbol(DataTypes.BOOL, False)
            elif type_attrib == 'nil':
                arg = ArgSymbol(DataTypes.NIL, None)
        elif typ == ArgLabel:
            arg = ArgLabel(element.text)
        elif typ == ArgType:
            if element.text == 'int':
                arg = ArgType(DataTypes.INT)
            elif element.text == 'string':
                arg = ArgType(DataTypes.STRING)
            elif element.text == 'bool':
                arg = ArgType(DataTypes.BOOL)
        return arg


#**************************** OPCODE INFO ****************************#
opcode_info = {
    'MOVE': [Move, 2, [ArgVariable, ArgSymbol]],
    'CREATEFRAME': [Createframe, 0, []],
    'PUSHFRAME': [Pushframe, 0, []],
    'POPFRAME': [Popframe, 0, []],
    'DEFVAR': [Defvar, 1, [ArgVariable]],
    'CALL': [Call, 1, [ArgLabel]],
    'RETURN': [Return, 0, []],
    'PUSHS': [PushS, 1, [ArgSymbol]],
    'POPS': [PopS, 1, [ArgVariable]],
    'ADD': [Add, 3, [ArgVariable, ArgSymbol, ArgSymbol]],
    'SUB': [Sub, 3, [ArgVariable, ArgSymbol, ArgSymbol]],
    'MUL': [Mul, 3, [ArgVariable, ArgSymbol, ArgSymbol]],
    'IDIV': [IDiv, 3, [ArgVariable, ArgSymbol, ArgSymbol]],
    'LT': [Lt, 3, [ArgVariable, ArgSymbol, ArgSymbol]],
    'GT': [Gt, 3, [ArgVariable, ArgSymbol, ArgSymbol]],
    'EQ': [Eq, 3, [ArgVariable, ArgSymbol, ArgSymbol]],
    'AND': [And, 3, [ArgVariable, ArgSymbol, ArgSymbol]],
    'OR': [Or, 3, [ArgVariable, ArgSymbol, ArgSymbol]],
    'NOT': [Not, 2, [ArgVariable, ArgSymbol]],
    'INT2CHAR': [Int2Char, 2, [ArgVariable, ArgSymbol]],
    'STRI2INT': [Stri2Int, 3, [ArgVariable, ArgSymbol, ArgSymbol]],
    'READ': [Read, 2, [ArgVariable, ArgType]],
    'WRITE': [Write, 1, [ArgSymbol]],
    'CONCAT': [Concat, 3, [ArgVariable, ArgSymbol, ArgSymbol]],
    'STRLEN': [Strlen, 2, [ArgVariable, ArgSymbol]],
    'GETCHAR': [Getchar, 3, [ArgVariable, ArgSymbol, ArgSymbol]],
    'SETCHAR': [Setchar, 3, [ArgVariable, ArgSymbol, ArgSymbol]],
    'TYPE': [Type, 2, [ArgVariable, ArgSymbol]],
    'LABEL': [Label, 1, [ArgLabel]],
    'JUMP': [Jump, 1, [ArgLabel]],
    'JUMPIFEQ': [JumpIfEq, 3, [ArgLabel, ArgSymbol, ArgSymbol]],
    'JUMPIFNEQ': [JumpIfNeq, 3, [ArgLabel, ArgSymbol, ArgSymbol]],
    'EXIT': [Exit, 1, [ArgSymbol]],
    'DPRINT': [DPrint, 1, [ArgSymbol]],
    'BREAK': [Break, 0, []]
}


#************************* SCRIPT PARAMETERS *************************#
class ParamHandler:
    argv = []
    opts = []
    source_file, input_file = None, None

    def __init__(self):
        self.argv = sys.argv[1:]
        try:
            self.opts, self.args = getopt.getopt(self.argv, "", ["help", "input=", "source="])
        except getopt.GetoptError:
            my_exit(ExitCodes.MISSING_PARAM, "GetOpt fail.")

        if ('--help', '') in self.opts:
            if len(self.argv) > 1:
                my_exit(ExitCodes.MISSING_PARAM, 'Wrong combination of script arguments.')
            else:
                print_help()
        if not (any('--source' in i for i in self.opts) and any('--source' in i for i in self.opts)):
            my_exit(ExitCodes.MISSING_PARAM, 'Missing at least one of the --source or --input arguments.')
        for opt, arg in self.opts:
            if opt == '--source':
                try:
                    self.source_file = open(arg, 'r')
                except (FileNotFoundError, IOError):
                    my_exit(ExitCodes.MISSING_PARAM, 'Can not read file or file not found of some parameter.')
            if opt == '--input':
                try:
                    self.input_file = open(arg, 'r')
                except (FileNotFoundError, IOError):
                    my_exit(ExitCodes.MISSING_PARAM, 'Can not read file or file not found of some parameter.')
        if self.source_file is not None and self.input_file is None:
            self.input_file = sys.stdin
        elif self.source_file is None and self.input_file is not None:
            self.source_file = sys.stdin

    def get_source(self):
        return self.source_file

    def get_input(self):
        return self.input_file


#******************************** MAIN ********************************#
if __name__ == '__main__':
    param_handler = ParamHandler()
    xml_parser = XmlParser(param_handler.get_source())
    xml_parser.parse()
    program = ProgramExecutor(xml_parser.inst_dict, param_handler.get_input())
    program.exec()
