package com.example.demo_websocket;

import org.springframework.stereotype.Service;

import java.time.LocalDate;
import java.time.LocalTime;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;

@Service
public class CommandService {

    private final Map<String, List<String>> data;
    public int questionResult;

    public CommandService() {
        data = Map.ofEntries(
                Map.entry("greeting", List.of(
                        "Hello", "Hi there", "Hey", "Hey there", "Good morning"
                )),
                Map.entry("time_inquiry", List.of(
                        "What is the time?", "Can you tell me the time?", "What time is it now?"
                )), Map.entry("date_inquiry", List.of(
                        "What is the date today?", "Can you tell me today's date?", "What's the date today?"
                )), Map.entry("weather", List.of(
                        "What's the weather like?", "How is the weather today?", "What's the weather forecast?"
                )), Map.entry("new_question", List.of(
                        "generate me a new question", "generate me a question", "ask me a mathematics question",
                        "generate me a new mathematics question", "generate me a mathematics question", "ask me a mathematics question",
                        "create me a new question", "create me a question", "create me a mathematics question"
                ))
        );
    }

    public String processCommand(String command) {
        var tokens = command.split(" ");
        var matchMap = new HashMap<>(Map.of(
                "greeting", 0,
                "time_inquiry", 0,
                "date_inquiry", 0,
                "weather", 0,
                "new_question", 0
        ));
        for (var entries : data.entrySet()) {
            var key = entries.getKey();
            var values = entries.getValue();
            for (var val : values)
                for (var token : tokens)
                    for (var valTokens : val.split(" ")) {
                        if (valTokens.equalsIgnoreCase(token)) {
                            var count = matchMap.get(key) + 1;
                            matchMap.put(key, count);
                        }
                    }
        }
        var mostRelatedTopic = "";
        var res = matchMap.values().stream()
                .reduce(0, Integer::sum);
        if (res != 0)
            mostRelatedTopic = matchMap.entrySet().stream()
                    .max((o1, o2) -> o1.getValue() - o2.getValue() >= 0 ? (o1.getValue().equals(o2.getValue()) ? 0 : 1) : -1)
                    .get().getKey();

        return switch (mostRelatedTopic) {
            case "greeting" -> "Hello! How can I help you today?";
            case "time_inquiry" -> "The current time is " + LocalTime.now();
            case "date_inquiry" -> "Today's date is " + LocalDate.now();
            case "new_question" -> "question=" + generateArithmeticQuestion();
            case "weather" -> "The weather today is sunny with a slight chance of rain.";
            default -> "I'm not sure how to help with that.";
        };
    }

    public String generateArithmeticQuestion() {
        var random = new Random();
        var num1 = random.nextInt(100) + 1;
        var num2 = random.nextInt(100) + 1;
        var operator = getRandomOperator(random);
        switch (operator) {
            case '+' -> questionResult = num1 + num2;
            case '-' -> questionResult = num1 > num2 ? num1 - num2 : num2 - num1;
            case '*' -> questionResult = num1 * num2;
            case '/' -> questionResult = num1 > num2 ? num1 / num2 : num2 / num1;
        }
        return num1 > num2
                ? num1 + " " + operator + " " + num2
                : num2 + " " + operator + " " + num1;
    }

    private char getRandomOperator(Random random) {
        var operators = new char[]{'+', '-', '*', '/'};
        var index = random.nextInt(operators.length);
        return operators[index];
    }


}
