#include "Game.hpp"

namespace options {
    t_options      g_optionsp1 = { 10, 1 };
    t_options      g_optionsp2 = { 10, 1 };
}

static bool       check_depth(int depth) {
    if (depth > 1) {
        return (true);
    }
    else
        return (false);
}

static void      get_depth(std::vector<int> depths) {
    if (depths.size() == 2) {
        options::g_optionsp1.depth = check_depth(depths[0]) ? depths[0] : options::g_optionsp1.depth;
        options::g_optionsp2.depth = check_depth(depths[1]) ? depths[1] : options::g_optionsp2.depth;
    }
    else if (depths.size() == 1) {
        options::g_optionsp1.depth = check_depth(depths[0]) ? depths[0] : options::g_optionsp1.depth;
    }
    else {
        return ;
    }
}

static int      check_algo_type(int algo_type, int player) {
    std::cout << "AI player " << player << ": ";
    switch (algo_type) {
        case 1:
            std::cout << "(1) default" << std::endl;
            return (1);
        case 2:
            std::cout << "(2) MinMax" << std::endl;
            return (2);
        case 3:
            std::cout << "(3) AlphaBeta" << std::endl;
            return (3);
        case 4:
            std::cout << "(4) MTDf" << std::endl;
            return (4);
        default:
            std::cout << "(1) default" << std::endl;
            return (1);
    }
}

static void      get_algo_type(std::vector<int> algo_types) {
    if (algo_types.size() == 2) {
        options::g_optionsp1.algo_type = check_algo_type(algo_types[0], 1);
        options::g_optionsp2.algo_type = check_algo_type(algo_types[1], 2);
    }
    else if (algo_types.size() == 1) {
        options::g_optionsp1.algo_type = check_algo_type(algo_types[0], 1);
    }
    else {
        return ;
    }
}

static void     print_options(void) {
    std::cout << "Welcome to:" << std::endl;
    std::cout << "\t ██████╗  ██████╗ ███╗   ███╗ ██████╗ ██╗  ██╗██╗   ██╗™" << std::endl;
    std::cout << "\t██╔════╝ ██╔═══██╗████╗ ████║██╔═══██╗██║ ██╔╝██║   ██║" << std::endl;
    std::cout << "\t██║  ███╗██║   ██║██╔████╔██║██║   ██║█████╔╝ ██║   ██║" << std::endl;
    std::cout << "\t██║   ██║██║   ██║██║╚██╔╝██║██║   ██║██╔═██╗ ██║   ██║" << std::endl;
    std::cout << "\t╚██████╔╝╚██████╔╝██║ ╚═╝ ██║╚██████╔╝██║  ██╗╚██████╔╝" << std::endl;
    std::cout << "\t ╚═════╝  ╚═════╝ ╚═╝     ╚═╝ ╚═════╝ ╚═╝  ╚═╝ ╚═════╝ " << std::endl;
    std::cout << "\tMade with ♥︎ by wwatkins and ademenet. Please enjoy." << std::endl;
    return ;
}

int             main(int argc, char **argv) {
    try {
        boost::program_options::options_description     desc("Options");
        boost::program_options::variables_map           vm;
        desc.add_options()
            ("help,h", "Print help options")
            ("ai,a", boost::program_options::value<std::vector<int> >()->multitoken(), "Choose AI algorithm:\n(1) default,\n(2) MinMax,\n(3) AlphaBeta,\n(4) MTDf")
            ("depth,d", boost::program_options::value<std::vector<int> >()->multitoken(), "Select the maximum depth");
        try {
            boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
            boost::program_options::notify(vm);
            if (vm.count("help")) {
                print_options();
                std::cout << desc << std::endl;
                return (0);
            }
            if (vm.count("ai")) {
                get_algo_type(vm["ai"].as<std::vector<int> >());
            }
            if (vm.count("depth")) {
                get_depth(vm["depth"].as<std::vector<int> >());
            }
        }
        catch(boost::program_options::error& e) {
            std::cerr << "Error: " << e.what() << std::endl << desc << std::endl;
            return (1);
        }
        Game    *game = new Game();
        game->loop(); // Start the game
    }
    catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl << std::endl;
        return (2);
    }
    return (0);
}
