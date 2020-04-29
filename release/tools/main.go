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
	TemplateName string
	Source       WaveFormat
	Target       WaveFormat
}

var (
	CMakeBytes  []byte
	CppBytesMap map[string][]byte
)

func main() {
	log.SetFlags(0)
	log.SetPrefix("error: ")

	if err := run(); err != nil {
		log.Fatal(err)
	}
}

func run() error {
	CppBytesMap = make(map[string][]byte)

	err := filepath.Walk("template", func(path string, info os.FileInfo, err error) error {
		if info.IsDir() || !strings.HasSuffix(path, ".cpp") {
			return nil
		}

		data, err := ioutil.ReadFile(path)

		if err != nil {
			return err
		}

		CppBytesMap[strings.TrimSuffix(info.Name(), ".cpp")] = data

		return nil
	})
	if err != nil {
		return err
	}

	CMakeBytes, err = ioutil.ReadFile(filepath.Join("template", "CMakeLists.txt"))

	if err != nil {
		return err
	}

	patterns, err := createTestPatterns("tests.csv")

	if err != nil {
		return err
	}

	cmakeTexts := []string{}
	playScripts := []string{}

	for _, pattern := range patterns {
		testName, err := createTest(pattern)

		if err != nil {
			return err
		}

		cmakeTexts = append(cmakeTexts, fmt.Sprintf("add_subdirectory(%s)", testName))
		playScripts = append(playScripts, fmt.Sprintf(
			"play -r %d -c %d -t s%d %s",
			pattern.Target.SamplesPerSec,
			pattern.Target.Channels,
			pattern.Target.BitsPerSample,
			filepath.Join("..", "build", "tests", testName, "output.raw"),
		))

	}
	if err := ioutil.WriteFile(filepath.Join("..", "tests", "CMakeLists.txt"), []byte(strings.Join(cmakeTexts, "\n")), 0644); err != nil {
		return err
	}
	if err := ioutil.WriteFile("playback.bash", []byte(strings.Join(playScripts, "\n")), 0644); err != nil {
		return err
	}
	return nil
}

func createTestPatterns(path string) ([]TestPattern, error) {
	csvFile, err := os.Open(path)

	if err != nil {
		return nil, err
	}

	defer csvFile.Close()

	// The test pattern CSV file must contains 7 columns below.
	// "test_name,source_samples_per_sec,source_channels,source_bits_per_sample,target_samples_per_sec,target_channels,target_bits_per_sample"
	const columns = 7

	patterns := []TestPattern{}
	reader := csv.NewReader(csvFile)

	for line := 1; ; line++ {
		values, err := reader.Read()

		if line == 1 {
			// Skip reading header.
			continue
		}
		if err == io.EOF {
			break
		}
		if err != nil {
			return nil, fmt.Errorf("%s (line:%d) csv.Reader.Read(): %v", line, path, err)
		}
		if len(values) != columns {
			return nil, fmt.Errorf("%s (line:%d) CSV must contains %d columns", path, line, columns)
		}

		var (
			sourceSamplesPerSec int
			sourceChannels      int
			sourceBitsPerSample int
			targetSamplesPerSec int
			targetChannels      int
			targetBitsPerSample int
		)

		n, err := fmt.Sscanf(
			strings.Join(values[1:], ","),
			"%d,%d,%d,%d,%d,%d",
			&sourceSamplesPerSec,
			&sourceChannels,
			&sourceBitsPerSample,
			&targetSamplesPerSec,
			&targetChannels,
			&targetBitsPerSample,
		)

		if err != nil {
			return nil, fmt.Errorf("%s (line:%d) fmt.Sscanf(): %v", path, line, err)
		}
		if n != columns-1 {
			return nil, fmt.Errorf("%s (line:%d) csv failed to parse", path, line)
		}

		patterns = append(patterns, TestPattern{
			TemplateName: values[0],
			Source:       WaveFormat{sourceSamplesPerSec, sourceChannels, sourceBitsPerSample},
			Target:       WaveFormat{targetSamplesPerSec, targetChannels, targetBitsPerSample},
		})
	}

	return patterns, nil
}

func createTest(pattern TestPattern) (string, error) {
	testName := fmt.Sprintf(
		"%sFrom%dHz%dch%dbitTo%dHz%dch%dbit",
		pattern.TemplateName,
		pattern.Source.SamplesPerSec,
		pattern.Source.Channels,
		pattern.Source.BitsPerSample,
		pattern.Target.SamplesPerSec,
		pattern.Target.Channels,
		pattern.Target.BitsPerSample,
	)

	outputDir := filepath.Join("..", "tests", testName)
	os.MkdirAll(outputDir, 0755)
	fmt.Println("CREATE", outputDir)

	inputFileName := fmt.Sprintf(
		filepath.Join("..", "..", "..", "assets", "%dHz%dch%dbit.wav"),
		pattern.Source.SamplesPerSec,
		pattern.Source.Channels,
		pattern.Source.BitsPerSample,
	)

	cppText := string(CppBytesMap[pattern.TemplateName])
	cppText = strings.Replace(cppText, "__INPUT_FILE__", inputFileName, -1)
	cppText = strings.Replace(cppText, "__OUTPUT_CHANNELS__", fmt.Sprint(pattern.Target.Channels), -1)
	cppText = strings.Replace(cppText, "__OUTPUT_SAMPLES_PER_SEC__", fmt.Sprint(pattern.Target.SamplesPerSec), -1)

	cppPath := filepath.Join(outputDir, "main_test.cpp")

	if err := ioutil.WriteFile(cppPath, []byte(cppText), 0644); err != nil {
		return testName, err
	}

	cmakeText := strings.Replace(string(CMakeBytes), "__TEST_NAME__", testName, -1)

	cmakePath := filepath.Join(outputDir, "CMakeLists.txt")

	if err := ioutil.WriteFile(cmakePath, []byte(cmakeText), 0644); err != nil {
		return testName, err
	}

	return testName, nil
}
