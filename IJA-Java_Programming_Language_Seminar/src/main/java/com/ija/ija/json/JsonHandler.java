package com.ija.ija.json;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.*;
import com.ija.ija.uml.ClassDiagram;

import java.io.*;
import java.util.stream.Collectors;

/**
 * @author Dominik Vágner, xvagne10
 * @author Ladislav Vašina, xvasin11
 */
public class JsonHandler {
    private static ObjectMapper objectMapper = getDefaultObjectMapper();

    private static ObjectMapper getDefaultObjectMapper() {
        ObjectMapper defaultObjectMapper = new ObjectMapper();
        defaultObjectMapper.configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
        return defaultObjectMapper;
    }

    public static JsonNode parse(String src) throws JsonProcessingException {
        return objectMapper.readTree(src);
    }

    public static <G> G fromJson(JsonNode node, Class<G> cls) throws JsonProcessingException {
        return objectMapper.treeToValue(node, cls);
    }

    public static JsonNode toJson(Object obj) {
        return objectMapper.valueToTree(obj);
    }

    public static String printNode(JsonNode node) throws JsonProcessingException {
        ObjectWriter objectWriter = objectMapper.writerWithDefaultPrettyPrinter();
        return objectWriter.writeValueAsString(node);
    }

    public static boolean writeToFile(ClassDiagram clsDiagram, String path) {
        JsonNode node = JsonHandler.toJson(clsDiagram);
        String fileName = String.format("%s%s.json", path, clsDiagram.getName());
        try {
            BufferedWriter bw = new BufferedWriter(new FileWriter(fileName));
            bw.write(JsonHandler.printNode(node));
            bw.close();
        } catch (IOException e) {
           return false;
        }
        return true;
    }

    public static ClassDiagram loadFromFile(String fileName) throws FileNotFoundException, JsonProcessingException {
        BufferedReader br = new BufferedReader(new FileReader(fileName), 131072);
        String content = br.lines().collect(Collectors.joining(System.lineSeparator()));
        JsonNode newNode = JsonHandler.parse(content);
        return JsonHandler.fromJson(newNode, ClassDiagram.class);
    }
}
