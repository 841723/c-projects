-- Neovim configuration for C/C++ and common web, scripting and systems languages.

vim.g.mapleader = " "
vim.g.maplocalleader = " "

-- General editor settings.
vim.opt.number = true
vim.opt.relativenumber = true
vim.opt.expandtab = true
vim.opt.shiftwidth = 4
vim.opt.tabstop = 4
vim.opt.smartindent = true
vim.opt.termguicolors = true
vim.opt.signcolumn = "yes"
vim.opt.cursorline = true
vim.opt.wrap = false
vim.opt.ignorecase = true
vim.opt.smartcase = true
vim.opt.updatetime = 250
vim.opt.timeoutlen = 400
vim.opt.splitright = true
vim.opt.splitbelow = true
vim.opt.scrolloff = 8
vim.opt.clipboard = "unnamedplus"

-- Use the standard data directory so the installer and Neovim share the same
-- lazy.nvim installation, including when XDG_DATA_HOME is set.
local lazypath = vim.fn.stdpath("data") .. "/lazy/lazy.nvim"
if not vim.uv.fs_stat(lazypath) then
    vim.fn.mkdir(vim.fn.fnamemodify(lazypath, ":h"), "p")
    local result = vim.fn.system({
        "git", "clone", "--filter=blob:none",
        "https://github.com/folke/lazy.nvim.git", lazypath,
    })
    if vim.v.shell_error ~= 0 then
        error("No se pudo instalar lazy.nvim: " .. result)
    end
end
vim.opt.rtp:prepend(lazypath)

local servers = {
    "clangd",        -- C and C++
    "lua_ls",        -- Lua
    "pyright",       -- Python
    "ts_ls",         -- JavaScript and TypeScript
    "bashls",        -- Bash and shell scripts
    "jsonls",        -- JSON
    "yamlls",        -- YAML
    "html",          -- HTML
    "cssls",         -- CSS, SCSS and LESS
    "rust_analyzer", -- Rust
}

require("lazy").setup({
    {
        "nvim-treesitter/nvim-treesitter",
        branch = "main",
        lazy = false,
        config = function()
            local parser_names = {
                "bash", "c", "cmake", "cpp", "css", "dockerfile", "html",
                "javascript", "json", "lua", "make", "markdown",
                "markdown_inline", "python", "query", "regex", "rust", "scss",
                "toml", "tsx", "typescript", "vim", "vimdoc", "yaml",
            }

            require("nvim-treesitter").install(parser_names)
            vim.api.nvim_create_autocmd("FileType", {
                callback = function(args)
                    -- A missing parser should not prevent editing that file.
                    pcall(vim.treesitter.start, args.buf)
                end,
            })
        end,
    },

    -- LSP support and automatic installation of language servers.
    {
        "neovim/nvim-lspconfig",
        dependencies = {
            "williamboman/mason.nvim",
            "WhoIsSethDaniel/mason-tool-installer.nvim",
            "hrsh7th/cmp-nvim-lsp",
        },
        config = function()
            local capabilities = require("cmp_nvim_lsp").default_capabilities()
            vim.lsp.config("*", { capabilities = capabilities })

            vim.lsp.config("clangd", {
                cmd = {
                    "clangd", "--background-index", "--clang-tidy",
                    "--completion-style=detailed", "--header-insertion=iwyu",
                },
            })
            vim.lsp.config("lua_ls", {
                settings = {
                    Lua = {
                        diagnostics = { globals = { "vim" } },
                        workspace = { checkThirdParty = false },
                        telemetry = { enable = false },
                    },
                },
            })

            vim.lsp.enable(servers)

            vim.api.nvim_create_autocmd("LspAttach", {
                callback = function(args)
                    local opts = { buffer = args.buf, silent = true }
                    vim.keymap.set("n", "gd", vim.lsp.buf.definition, opts)
                    vim.keymap.set("n", "gD", vim.lsp.buf.declaration, opts)
                    vim.keymap.set("n", "gr", vim.lsp.buf.references, opts)
                    vim.keymap.set("n", "gi", vim.lsp.buf.implementation, opts)
                    vim.keymap.set("n", "K", vim.lsp.buf.hover, opts)
                    vim.keymap.set("n", "<leader>rn", vim.lsp.buf.rename, opts)
                    vim.keymap.set("n", "<leader>ca", vim.lsp.buf.code_action, opts)
                end,
            })
        end,
    },

    { "williamboman/mason.nvim", opts = {} },

    -- Install language servers and formatters via Mason.
    {
        "WhoIsSethDaniel/mason-tool-installer.nvim",
        dependencies = { "williamboman/mason.nvim" },
        opts = {
            ensure_installed = {
                "clangd", "lua-language-server", "pyright",
                "typescript-language-server", "bash-language-server",
                "json-lsp", "yaml-language-server", "html-lsp", "css-lsp",
                "rust-analyzer", "codelldb", "clang-format", "stylua",
                "black", "prettier", "shfmt",
            },
            auto_update = false,
            run_on_start = true,
        },
    },
    {
        "stevearc/conform.nvim",
        event = { "BufWritePre" },
        opts = {
            formatters_by_ft = {
                c = { "clang_format" },
                cpp = { "clang_format" },
                lua = { "stylua" },
                python = { "black" },
                javascript = { "prettier" },
                javascriptreact = { "prettier" },
                typescript = { "prettier" },
                typescriptreact = { "prettier" },
                json = { "prettier" },
                html = { "prettier" },
                css = { "prettier" },
                yaml = { "prettier" },
                markdown = { "prettier" },
                sh = { "shfmt" },
                bash = { "shfmt" },
                _ = { "trim_whitespace" },
            },
            format_on_save = {
                timeout_ms = 1000,
                lsp_format = "fallback",
            },
        },
        keys = {
            {
                "<leader>f",
                function()
                    require("conform").format({ async = true, lsp_format = "fallback" })
                end,
                mode = "n",
                desc = "Format buffer",
            },
        },
    },

    -- Completion, snippets and completion sources.
    {
        "hrsh7th/nvim-cmp",
        event = "InsertEnter",
        dependencies = {
            "hrsh7th/cmp-nvim-lsp",
            "hrsh7th/cmp-buffer",
            "hrsh7th/cmp-path",
            "L3MON4D3/LuaSnip",
            "saadparwaiz1/cmp_luasnip",
            "rafamadriz/friendly-snippets",
        },
        config = function()
            local cmp = require("cmp")
            require("luasnip.loaders.from_vscode").lazy_load()
            cmp.setup({
                snippet = {
                    expand = function(args)
                        require("luasnip").lsp_expand(args.body)
                    end,
                },
                mapping = cmp.mapping.preset.insert({
                    ["<C-Space>"] = cmp.mapping.complete(),
                    ["<CR>"] = cmp.mapping.confirm({ select = true }),
                    ["<Tab>"] = cmp.mapping.select_next_item(),
                    ["<S-Tab>"] = cmp.mapping.select_prev_item(),
                }),
                sources = cmp.config.sources({
                    { name = "nvim_lsp" },
                    { name = "luasnip" },
                    { name = "path" },
                }, {
                    { name = "buffer" },
                }),
            })
        end,
    },

    {
        "nvim-tree/nvim-tree.lua",
        dependencies = { "nvim-tree/nvim-web-devicons" },
        config = function()
            require("nvim-tree").setup({})
            vim.keymap.set("n", "<leader>e", ":NvimTreeToggle<CR>", {
                silent = true, desc = "Toggle file tree",
            })
        end,
    },

    {
        "nvim-telescope/telescope.nvim",
        dependencies = { "nvim-lua/plenary.nvim" },
        config = function()
            local telescope = require("telescope.builtin")
            vim.keymap.set("n", "<leader>ff", telescope.find_files, { desc = "Find files" })
            vim.keymap.set("n", "<leader>fg", telescope.live_grep, { desc = "Search text" })
            vim.keymap.set("n", "<leader>fb", telescope.buffers, { desc = "Find buffers" })
            vim.keymap.set("n", "<leader>fh", telescope.help_tags, { desc = "Help" })
        end,
    },

    {
        "mfussenegger/nvim-dap",
        config = function()
            local dap = require("dap")
            dap.adapters.codelldb = {
                type = "server",
                port = "${port}",
                executable = {
                    command = vim.fn.stdpath("data") .. "/mason/bin/codelldb",
                    args = { "--port", "${port}" },
                },
            }

            local c_config = {
                {
                    name = "Launch executable",
                    type = "codelldb",
                    request = "launch",
                    program = function()
                        return vim.fn.input("Executable: ", vim.fn.getcwd() .. "/", "file")
                    end,
                    cwd = "${workspaceFolder}",
                    stopOnEntry = true,
                },
            }
            dap.configurations.c = c_config
            dap.configurations.cpp = c_config
            dap.configurations.rust = c_config

            vim.keymap.set("n", "<F5>", dap.continue, { desc = "Debug: Continue" })
            vim.keymap.set("n", "<F10>", dap.step_over, { desc = "Debug: Step over" })
            vim.keymap.set("n", "<F11>", dap.step_into, { desc = "Debug: Step into" })
            vim.keymap.set("n", "<F12>", dap.step_out, { desc = "Debug: Step out" })
            vim.keymap.set("n", "<leader>b", dap.toggle_breakpoint, { desc = "Toggle breakpoint" })
        end,
    },
})

vim.diagnostic.config({
    virtual_text = true,
    signs = true,
    underline = true,
    update_in_insert = false,
    severity_sort = true,
    float = { border = "rounded" },
})

vim.keymap.set("n", "<leader>w", ":write<CR>", { silent = true, desc = "Save" })
vim.keymap.set("n", "<leader>q", ":quit<CR>", { silent = true, desc = "Quit" })
