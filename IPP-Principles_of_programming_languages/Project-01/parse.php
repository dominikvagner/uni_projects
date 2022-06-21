<?php
ini_set("display_errors", "stderr");

// Arguments
$options = getopt("", ["help"]);
if (count($options) == 1 and $argc <= 2) {
    if (array_key_exists('help', $options)) {
        echo <<<'EOD'
Script for parsing IPPcode22 files into XML.

USAGE:
    php8.1 [filename (parse.php)] < [source code input (*.src)] > [XML output (*.out)] &&
    echo $? > [return code (*.rc)]
EOD . PHP_EOL;
        exit(0);
    }
} else if ($argc >= 2) {
    fprintf(STDERR, 'Too many or wrong arguments.' . "\xA");
    exit(10);
}

/*** MAIN ***/
// Input and header check.
$line = fgets(STDIN);
if ($line == false) { fprintf(STDERR, 'Error when opening input files.' . "\xA"); exit(11); }
while (strlen(rtrim($line)) == 0 || strlen(trim_comment($line)) == 0) {
    $line = fgets(STDIN);
}
$line = trim(trim_comment($line));
if (strcasecmp($line, '.IPPcode22') != 0) {
    fprintf(STDERR, 'Faulty or missing header in source code.' . "\xA");
    exit(21);
}

$xml = new DOMDocument('1.0', 'UTF-8');
$program = $xml->createElement('program');
$lang = $xml->createAttribute('language');
$lang->value = 'IPPcode22';
$program->appendChild($lang);

// Loop processing each line until the input isn't empty.
$order = 1;
while ($line = fgets(STDIN)) {
    $line = trim($line);
    $line = trim_comment($line);
    if (strlen($line) == 0)
        continue;
    if (!process_line($line, $xml, $program, $order)) {
        fprintf(STDERR, 'Lexical or syntactic error.' . "\xA");
        exit(23);
    }
    $order++;
}

$xml->appendChild($program);
echo $xml->saveXML();
exit(0);

/*** FUNCTIONS ***/
function trim_comment($s) {
    if (!str_contains($s, '#')) {
        return $s;
    } else {
        if (($s[0] == '#'))
            return "";
        else
            return trim(substr($s, 0, strpos($s, '#')));
    }
}
function process_line($line, $xml, $program, $order) {
    $split = preg_split('/[ ]+/', $line);
    switch (strtoupper($split[0])) {
        // Without parameters
        case 'CREATEFRAME':
        case 'PUSHFRAME':
        case 'POPFRAME':
        case 'RETURN':
        case 'BREAK':
            if (count($split) == 1) {
                $new_instruction = create_xml_instruction($xml, $order, strtoupper($split[0]));
                $program->appendChild($new_instruction);
                return true;
            }
            return false;
        // <label>
        case 'CALL':
        case 'LABEL':
        case 'JUMP':
            if (count($split) == 2) {
                if (check_label($split[1])) {
                    $new_instruction = create_xml_instruction($xml, $order, strtoupper($split[0]));
                    $arg1 = create_xml_arg($xml, 'arg1', $split[1], true);
                    $new_instruction->appendChild($arg1);
                    $program->appendChild($new_instruction);
                    return true;
                }
            } return false;
        // <var>
        case 'DEFVAR':
        case 'POPS':
            if (count($split) == 2) {
                if (check_var($split[1])) {
                    $new_instruction = create_xml_instruction($xml, $order, strtoupper($split[0]));
                    $arg1 = create_xml_arg($xml, 'arg1', $split[1], false);
                    $new_instruction->appendChild($arg1);
                    $program->appendChild($new_instruction);
                    return true;
                }
            }
            return false;
        // <symb>
        case 'PUSHS':
        case 'WRITE':
        case 'EXIT':
        case 'DPRINT':
            if (count($split) == 2) {
                if (check_symb($split[1])) {
                    $new_instruction = create_xml_instruction($xml, $order, strtoupper($split[0]));
                    $arg1 = create_xml_arg($xml, 'arg1', $split[1], false);
                    $new_instruction->appendChild($arg1);
                    $program->appendChild($new_instruction);
                    return true;
                }
            }
            return false;
        // <var> <symb>
        case 'MOVE':
        case 'INT2CHAR':
        case 'STRLEN':
        case 'NOT':
        case 'TYPE':
            if (count($split) == 3) {
                if (check_var($split[1]) and check_symb($split[2])) {
                    $new_instruction = create_xml_instruction($xml, $order, strtoupper($split[0]));
                    $arg1 = create_xml_arg($xml, 'arg1', $split[1], false);
                    $arg2 = create_xml_arg($xml, 'arg2', $split[2], false);
                    $new_instruction->appendChild($arg1);
                    $new_instruction->appendChild($arg2);
                    $program->appendChild($new_instruction);
                    return true;
                }
            }
            return false;
        // <var> <type>
        case 'READ':
            if (count($split) == 3) {
                if (check_var($split[1]) and preg_match('/^(string|int|bool)$/', $split[2])) {
                    $new_instruction = create_xml_instruction($xml, $order, strtoupper($split[0]));
                    $arg1 = create_xml_arg($xml, 'arg1', $split[1], false);
                    $arg2 = create_xml_arg($xml, 'arg2', $split[2], false);
                    $new_instruction->appendChild($arg1);
                    $new_instruction->appendChild($arg2);
                    $program->appendChild($new_instruction);
                    return true;
                }
            }
            return false;
        // <var> <symb1> <symb2>
        case 'ADD':
        case 'SUB':
        case 'MUL':
        case 'IDIV':
        case 'LT':
        case 'GT':
        case 'EQ':
        case 'AND':
        case 'OR':
        case 'STRI2INT':
        case 'CONCAT':
        case 'GETCHAR':
        case 'SETCHAR':
            if (count($split) == 4) {
                if (check_var($split[1]) and check_symb($split[2]) and check_symb($split[3])) {
                    $new_instruction = create_xml_instruction($xml, $order, strtoupper($split[0]));
                    $arg1 = create_xml_arg($xml, 'arg1', $split[1], false);
                    $arg2 = create_xml_arg($xml, 'arg2', $split[2], false);
                    $arg3 = create_xml_arg($xml, 'arg3', $split[3], false);
                    $new_instruction->appendChild($arg1);
                    $new_instruction->appendChild($arg2);
                    $new_instruction->appendChild($arg3);
                    $program->appendChild($new_instruction);
                    return true;
                }
            }
            return false;
        // <label> <symb1> <symb2>
        case 'JUMPIFEQ':
        case 'JUMPIFNEQ':
            if (count($split) == 4) {
                if (check_label($split[1]) and check_symb($split[2]) and check_symb($split[3])) {
                    $new_instruction = create_xml_instruction($xml, $order, strtoupper($split[0]));
                    $arg1 = create_xml_arg($xml, 'arg1', $split[1], true);
                    $arg2 = create_xml_arg($xml, 'arg2', $split[2], false);
                    $arg3 = create_xml_arg($xml, 'arg3', $split[3], false);
                    $new_instruction->appendChild($arg1);
                    $new_instruction->appendChild($arg2);
                    $new_instruction->appendChild($arg3);
                    $program->appendChild($new_instruction);
                    return true;
                }
            }
            return false;
        default:
            fprintf(STDERR, 'Unknown or wrong opcode in IPPcode22.' . "\xA");
            exit(22);
    }
}
function create_xml_instruction($xml, $ord, $opc) {
    $instruction = $xml->createElement('instruction');
    $order = $xml->createAttribute('order');
    $order->value = $ord;
    $instruction->appendChild($order);
    $opcode = $xml->createAttribute('opcode');
    $opcode->value = $opc;
    $instruction->appendChild($opcode);
    return $instruction;
}
function create_xml_arg($xml, $name, $txt, $is_label) {
    $type = $xml->createAttribute('type');
    if ($is_label) {
        $arg = $xml->createElement($name, htmlspecialchars($txt, ENT_XML1));
        $type->value = 'label';
    } else if (!strpos($txt,'@')) {
        $arg = $xml->createElement($name, htmlspecialchars($txt, ENT_XML1));
        $type->value = 'type';
    } else {
        $split = explode('@', $txt, 2);
        if (preg_match('/^(GF|LF|TF)$/', $split[0])) {
            $arg = $xml->createElement($name, htmlspecialchars($txt, ENT_XML1));
            $type->value = 'var';
        } else {
            $arg = $xml->createElement($name, htmlspecialchars($split[1], ENT_XML1));
            $type->value = $split[0];
        }
    }
    $arg->appendChild($type);
    return $arg;
}
function check_label($label) {
    if (preg_match('/^([a-zA-Z]|_|-|\$|&|%|\*|!|\?)([a-zA-Z0-9]|_|-|\$|&|%|\*|!|\?)*$/', $label))
        return true;
    else
        return false;
}
function check_var($var) {
    if (preg_match('/^(GF|LF|TF)@([a-zA-Z]|_|-|\$|&|%|\*|!|\?)([a-zA-Z0-9]|_|-|\$|&|%|\*|!|\?)*$/', $var))
        return true;
    else
        return false;
}
function check_symb($symb) {
    $split_symb = explode('@', $symb, 2);
    if (preg_match('/^(GF|LF|TF)$/', $split_symb[0])) {
        if (preg_match('/^([a-zA-Z]|_|-|\$|&|%|\*|!|\?)([a-zA-Z0-9]|_|-|\$|&|%|\*|!|\?)*$/', $split_symb[1]))
            return true;
        else
            return false;
    } else if ($split_symb[0] == 'nil') {
        if ($split_symb[1] == 'nil')
            return true;
        else
            return false;
    } else if ($split_symb[0] == 'bool') {
        if (preg_match('/^(true|false)$/', $split_symb[1]))
            return true;
        else
            return false;
    } else if ($split_symb[0] == 'int') {
        if (preg_match('/^(-|\+)?\d+$/', $split_symb[1]))
            return true;
        else
            return false;
    } else if ($split_symb[0] == 'string') {
        if (preg_match('/(\p{L}|\\\d{3}|<|>|\/|-|_|\$|&|%|\*|!|\?)*/u', $split_symb[1])) {
            if (strlen($split_symb[1]) == 0)
                return true;
            $pos = 0;
            while (strpos($split_symb[1], '\\', $pos + 1) != false) {
                $pos = strpos($split_symb[1], '\\', $pos + 1);
                if (($pos + 3) > (strlen($split_symb[1]) - 1)) {
                    return false;
                }
                if (!is_numeric(substr($split_symb[1], $pos + 1, 3))) {
                    return false;
                }
            }
            return true;
        } else {
            return false;
        }
    }
    else {
        return false;
    }
}