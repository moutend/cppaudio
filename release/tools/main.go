package main

import (
	"encoding/csv"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"strings"
)

type WaveFormat struct {
	SamplesPerSec int
	Channels      int
	BitsPerSample int
}

type TestPattern struct {
	Source WaveFormat
	Target WaveFormat
}

var (
	CppBytes   []byte
	CMakeBytes []byte
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("error: ")

	if err := run(); err != nil {
		log.Fatal(err)
	}
}

func run() (err error) {
	CppBytes, err = ioutil.ReadFile(filepath.Join("template", "main_test.cpp"))

	if err != nil {
		return err
	}

	CMakeBytes, err = ioutil.ReadFile(filepath.Join("template", "CMakeLists.txt"))

	if err != nil {
		return err
	}

	patterns, err := readPatterns("pattern.csv")

	if err != nil {
		return err
	}

	cmakeTexts := []string{}
	playScripts := []string{"#!/bin/bash"}

	for _, pattern := range patterns {
		testName := fmt.Sprintf(
			"TestLauncherEngineFrom%dHz%dch%dbitTo%dHz%dch%dbit",
			pattern.Source.SamplesPerSec,
			pattern.Source.Channels,
			pattern.Source.BitsPerSample,
			pattern.Target.SamplesPerSec,
			pattern.Target.Channels,
			pattern.Target.BitsPerSample,
		)

		if err := createTest(testName, pattern); err != nil {
			return err
		}

		cmakeTexts = append(cmakeTexts, fmt.Sprintf("add_subdirectory(%s)", testName))

		generatedFilePath := filepath.Join("..", "build", "tests", testName, "output.raw")
		playScripts = append(playScripts, fmt.Sprintf(
			"play -r %d -c %d -t s%d %s\n",
			pattern.Target.SamplesPerSec,
			pattern.Target.Channels,
			pattern.Target.BitsPerSample,
			generatedFilePath,
		))
	}
	if err := ioutil.WriteFile(filepath.Join("..", "tests", "CMakeLists.txt"), []byte(strings.Join(cmakeTexts, "\n")), 0644); err != nil {
		return err
	}
	if err := ioutil.WriteFile("play.bash", []byte(strings.Join(playScripts, "\n")), 0644); err != nil {
		return err
	}

	return nil
}

func readPatterns(path string) ([]TestPattern, error) {
	csvFile, err := os.Open(path)

	if err != nil {
		return nil, err
	}

	defer csvFile.Close()

	patterns := []TestPattern{}
	reader := csv.NewReader(csvFile)

	for line := 1; ; line++ {
		values, err := reader.Read()

		if line == 1 {
			continue // Skip reading header.
		}
		if err == io.EOF {
			break
		}
		if err != nil {
			return nil, fmt.Errorf("pattern.csv (line:%d) %v", line, err)
		}
		if len(values) != 6 {
			return nil, fmt.Errorf("pattern.csv (line:%d) CSV must contains 6 columns", line)
		}

		var sourceSamplesPerSec, sourceChannels, sourceBitsPerSample, targetSamplesPerSec, targetChannels, targetBitsPerSample int

		n, err := fmt.Sscanf(
			strings.Join(values, ","),
			"%d,%d,%d,%d,%d,%d",
			&sourceSamplesPerSec,
			&sourceChannels,
			&sourceBitsPerSample,
			&targetSamplesPerSec,
			&targetChannels,
			&targetBitsPerSample,
		)

		if err != nil {
			return nil, fmt.Errorf("pattern.csv (line:%d) %v", line, err)
		}
		if n != 6 {
			return nil, fmt.Errorf("pattern.csv (line:%d) csv failed to parse", line)
		}

		patterns = append(patterns, TestPattern{
			Source: WaveFormat{sourceSamplesPerSec, sourceChannels, sourceBitsPerSample},
			Target: WaveFormat{targetSamplesPerSec, targetChannels, targetBitsPerSample},
		})
	}

	return patterns, nil
}

func createTest(testName string, pattern TestPattern) error {
	outputDir := filepath.Join("..", "tests", testName)
	os.MkdirAll(outputDir, 0755)
	fmt.Println("CREATE", outputDir)

	inputFileName := fmt.Sprintf(
		"%dHz%dch%dbit.wav",
		pattern.Source.SamplesPerSec,
		pattern.Source.Channels,
		pattern.Source.BitsPerSample,
	)

	cppText := string(CppBytes)
	cppText = strings.Replace(cppText, "__INPUT_FILE__", inputFileName, -1)
	cppText = strings.Replace(cppText, "__OUTPUT_CHANNELS__", fmt.Sprint(pattern.Target.Channels), -1)
	cppText = strings.Replace(cppText, "__OUTPUT_SAMPLES_PER_SEC__", fmt.Sprint(pattern.Target.SamplesPerSec), -1)

	cppPath := filepath.Join(outputDir, "main_test.cpp")

	if err := ioutil.WriteFile(cppPath, []byte(cppText), 0644); err != nil {
		return err
	}

	cmakeText := strings.Replace(string(CMakeBytes), "__TEST_NAME__", testName, -1)

	cmakePath := filepath.Join(outputDir, "CMakeLists.txt")

	if err := ioutil.WriteFile(cmakePath, []byte(cmakeText), 0644); err != nil {
		return err
	}

	return nil
}
