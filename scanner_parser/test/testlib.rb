# DO NOT MODIFY THIS FILE!

require 'singleton'

class String
	def green
		"\e[#{32}m#{self}\e[0m"
	end

	def red
		"\e[#{31}m#{self}\e[0m"
	end
end

class TestCase
	include Singleton

	class Config
		def initialize master
			@master = master
		end

		def testing key
			@master.testing = key
		end

		def executable key
			@master.executable = key
		end

		def to action, param
			@master.params[action] = param
		end
	end

	class Test
		def initialize master, mode
			@master = master
			@mode = mode
			@result = ""
			@passed = 0
			@failed = 0
		end

		def result
			@result
		end

		def stats
			puts "\n\t\t\t\t\t\tPassed: #{@passed} of #{@passed+@failed}."
		end
		
		def pass test, rexp = ""
			rexp = rexp.gsub("\n", "\\n")
			if @master.action == :test
				res = run test, @mode

				if res == rexp
					puts "[  OK  ] #{test} passed.".green
					@passed += 1
				else
					@failed += 1
					puts "[ FAIL ] #{test} failed.".red
					puts "Expected:\n".red
					puts "\t#{rexp}\n\n".red
					puts "Got:\n".red
					puts "\t#{res}\n\n".red
					puts "---------------------------------------".red
				end
			elsif @master.action == :generate
				res = check test, @mode
				if res.include? "error"
					puts "Warning: possible oracle error with #{test}!"
				end
				@result << "\tpass \"#{test}\", \"#{res}\"\n"		
			end
		end

		def fail test, rexp = ""
			rexp = rexp.gsub("\n", "\\n")
                        if @master.action == :test
                                res = run test, @mode

                                if res == rexp
					@passed += 1
                                        puts "[  OK  ] #{test} passed.".green
                                else
					@failed += 1
                                        puts "[ FAIL ] #{test} failed.".red
                                        puts "Expected:\n".red
                                        puts "\t#{rexp}\n\n".red
                                        puts "Got:\n".red
                                        puts "\t#{res}\n\n".red
                                        puts "---------------------------------------".red
                                end
                        elsif @master.action == :generate
                                res = check test, @mode
                                unless res.include? "error"
                                        puts "Warning: possible oracle error with #{test}!"
                                end
                                @result << "\tfail \"#{test}\", \"#{res}\"\n"
                        end
		end

		private

		def check test, mode
			`echo -n "#{test}" | #{@master.executable} #{@master.params[mode]} 2>&1`.strip.gsub("\n", "\\n")
		end

		def run test, mode
			`echo -n "#{test}" | #{@master.testing} #{@master.params[mode]} 2>&1`.strip.gsub("\n", "\\n")
		end
	end

	attr_accessor :config, :executable, :params, :suite, :tests, :testing, :action

	def initialize
		@config = Config.new(self)
		@suite = {}
		@tests = []
		@executable = nil
		@testing = nil
		@params = {}
		@action = ""
	end

	def add what, &block
		@suite[what] = block
		@tests.push what
	end
end

def prepare(&b)
	TestCase::Config.new(TestCase.instance).instance_eval &b
end

def doing what, &b
	TestCase.instance.add(what, &b)
end

def test!
	TestCase.instance.action = :test

	TestCase.instance.tests.each do |mode|
		puts "\n**** Running test group #{mode}:\n\n"
		test = TestCase::Test.new(TestCase.instance, mode)
		test.instance_eval &TestCase.instance.suite[mode]
		test.stats
	end
end

def generate!
	TestCase.instance.action = :generate

	result = ""
	result << "require_relative 'testlib'\n\n"
	result << "prepare do\n"
	result << "\texecutable \"#{TestCase.instance.executable}\"\n"
	result << "\ttesting \"#{TestCase.instance.testing}\"\n"
	result << "\n\n"

	TestCase.instance.params.each_key do |key|
		result << "\tto :#{key}, \"#{TestCase.instance.params[key]}\"\n"
	end	

	result << "end\n\n"

	TestCase.instance.tests.each do |mode|
		result << "doing :#{mode} do\n"
		test = TestCase::Test.new(TestCase.instance, mode)
		test.instance_eval &TestCase.instance.suite[mode]
		result << test.result
		result << "\n"
		result << "end\n\n"
	end
	
	result << "\n\ntest!\n"

	puts result
end

